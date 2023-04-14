from pyprd.chip_data import chip_data as cd
from pyprd.util.hash import hash_string
from pyprd.util.model_ec import supported as supported_model_ec

# -----------------------------------------------------------------------------
# Generic tools to convert to data bytes


def _bin(num_bytes: int, value: int) -> bytes:
    """Converts integers to bytes in big endian format."""
    return value.to_bytes(num_bytes, "big")


def _hash(num_bytes: int, string: str) -> bytes:
    """Hashes a string and converts to bytes."""
    return _bin(num_bytes, hash_string(num_bytes, string))


def _num(num_bytes: int, iterable: iter) -> bytes:
    """Gets the length of an iterable and converts to bytes."""
    return _bin(num_bytes, len(iterable))


# -----------------------------------------------------------------------------
# General fields with specific byte sizes


def _model_ec(value: int) -> bytes:
    return _bin(4, value)


def _version(value: int) -> bytes:
    return _bin(1, value)


def _node_name(name: str) -> bytes:
    return _hash(2, name)


def _num_nodes(iterable: iter) -> bytes:
    return _num(2, iterable)


def _reg_name(name: str) -> bytes:
    return _hash(3, name)


def _num_regs(iterable: iter) -> bytes:
    return _num(3, iterable)


def _inst(value: int) -> bytes:
    return _bin(1, value)


def _num_inst(iterable: iter) -> bytes:
    return _num(1, iterable)


def _pos(value: int) -> bytes:
    return _bin(1, value)


def _reg_type(value: str) -> bytes:
    m = {"SCOM": 1, "IDSCOM": 2}
    return _bin(1, m[value])


def _reg_flags(reg_access: str) -> bytes:
    m = {"RW": 0xC0, "RO": 0x80, "WO": 0x40}
    return _bin(1, m[reg_access])


def _reg_addr(reg_type: str, reg_addr: int) -> bytes:
    m = {"SCOM": 4, "IDSCOM": 8}
    return _bin(m[reg_type], reg_addr)


def _reg_val(reg_type: str, reg_val: int) -> bytes:
    m = {"SCOM": 8, "IDSCOM": 8}
    return _bin(m[reg_type], reg_val)


def _attn_type(value: str) -> bytes:
    m = {"CHIP_CS": 1, "UNIT_CS": 2, "RECOV": 3, "SP_ATTN": 4, "HOST_ATTN": 5}
    return _bin(1, m[value])


def _num_attn_types(iterable: iter) -> bytes:
    return _num(1, iterable)


# -----------------------------------------------------------------------------
# Isolation node capture register support


def _cap_regs(
    node_inst: int,
    node_cap_groups: list,
    node_bits: dict,
    base_cap_groups: dict,
) -> list:
    out = []

    for group in node_cap_groups:
        for reg in base_cap_groups[group.group_name]:
            out.append(
                _reg_name(reg.reg_name)
                + _inst(reg.reg_inst[group.group_inst[node_inst]])
                + _pos(0xFF)
            )

    for pos, bit in node_bits.items():
        for group in bit.capture_groups:
            for reg in base_cap_groups[group.group_name]:
                out.append(
                    _reg_name(reg.reg_name)
                    + _inst(reg.reg_inst[group.group_inst[node_inst]])
                    + _pos(pos)
                )

    return out


def _num_cap_regs(iterable: iter) -> bytes:
    return _num(1, iterable)


# -----------------------------------------------------------------------------
# Isolation node rule and expression support


def _expr_type(value: int) -> bytes:
    return _bin(1, value)


def _num_exprs(iterable: iter) -> bytes:
    return _num(1, iterable)


def _shift_value(value: int) -> bytes:
    return _bin(1, value)


def _expr(node_inst: int, expr: object) -> bytes:
    data = bytes()

    if "reg" == expr.expr_type:
        data += _expr_type(0x01)
        data += _reg_name(expr.reg_name)
        data += _inst(expr.reg_inst[node_inst] if expr.reg_inst else node_inst)

    elif "int" == expr.expr_type:
        data += _expr_type(0x02)
        data += _reg_val(expr.reg_type, expr.int_value)

    elif "and" == expr.expr_type:
        data += _expr_type(0x10)
        data += _num_exprs(expr.exprs)
        for e in expr.exprs:
            data += _expr(node_inst, e)

    elif "or" == expr.expr_type:
        data += _expr_type(0x11)
        data += _num_exprs(expr.exprs)
        for e in expr.exprs:
            data += _expr(node_inst, e)

    elif "not" == expr.expr_type:
        data += _expr_type(0x12)
        data += _expr(node_inst, expr.expr)

    elif "lshift" == expr.expr_type:
        data += _expr_type(0x13)
        data += _shift_value(expr.shift_value)
        data += _expr(node_inst, expr.expr)

    elif "rshift" == expr.expr_type:
        data += _expr_type(0x14)
        data += _shift_value(expr.shift_value)
        data += _expr(node_inst, expr.expr)

    return data


def _rules(node_inst: int, rules: list) -> list:
    out = []

    for rule in rules:
        if node_inst in rule.node_inst:
            for attn_type in rule.attn_type:
                out.append(_attn_type(attn_type) + _expr(node_inst, rule.expr))

    return out


def _num_rules(iterable: iter) -> bytes:
    return _num(1, iterable)


# -----------------------------------------------------------------------------
# Isolation node child node support


def _child_nodes(node_inst: int, bits: dict) -> list:
    out = []

    for pos, bit in bits.items():
        if bit.child_node:
            name = bit.child_node["name"]

            # The bit instance map is optional and matches the node instance if
            # not present.
            inst = node_inst
            if "inst" in bit.child_node:
                inst = bit.child_node["inst"][node_inst]

            out.append(_pos(pos) + _node_name(name) + _inst(inst))

    return out


def _num_child_nodes(iterable: iter) -> bytes:
    return _num(1, iterable)


# -----------------------------------------------------------------------------
# Public functions


def binary_encode(model_ec: str, base: cd.Base, fp: object):
    """
    Encodes a Chip Data object into binary and writes to the given file
    pointer. Note that the Chip Data Base object could describe more than one
    model/EC, but the Chip Data binary files only target one model/EC.
    Therefore, the user must specify which model/EC to target.
    """
    assert model_ec in supported_model_ec
    assert model_ec in base.model_ec

    data = bytes()

    # Header information.
    data += "CHIPDATA".encode()
    data += _model_ec(supported_model_ec[model_ec].id)
    data += _version(2)

    # Register information.
    data += "REGS".encode()
    data += _num_regs(base.registers)

    for reg_name, register in sorted(base.registers.items()):
        data += _reg_name(reg_name)
        data += _reg_type(register.reg_type)
        data += _reg_flags(register.access)
        data += _num_inst(register.instances)

        for reg_inst, reg_addr in sorted(register.instances.items()):
            data += _inst(reg_inst)
            data += _reg_addr(register.reg_type, reg_addr)

    # Isolation node information.
    data += "NODE".encode()
    data += _num_nodes(base.isolation_nodes)

    for node_name, iso_node in sorted(base.isolation_nodes.items()):
        data += _node_name(node_name)
        data += _reg_type(iso_node.reg_type)
        data += _num_inst(iso_node.instances)

        for node_inst in sorted(iso_node.instances):
            cap_regs = _cap_regs(
                node_inst,
                iso_node.capture_groups,
                iso_node.bits,
                base.capture_groups,
            )
            rules = _rules(node_inst, iso_node.rules)
            child_nodes = _child_nodes(node_inst, iso_node.bits)

            data += _inst(node_inst)
            data += _num_cap_regs(cap_regs)
            data += _num_rules(rules)
            data += _num_child_nodes(child_nodes)

            for cap_reg in cap_regs:
                data += cap_reg

            for rule in rules:
                data += rule

            for child_node in child_nodes:
                data += child_node

    # Root node node information.
    data += "ROOT".encode()
    data += _num_attn_types(base.root_nodes)

    for attn_type, root_node in sorted(base.root_nodes.items()):
        data += _attn_type(attn_type)
        data += _node_name(root_node.name)
        data += _inst(root_node.inst)

    fp.write(data)
