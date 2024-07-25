import json

from pyprd.chip_data import chip_data as cd

# Encoder support --------------------------------------------------------------
# This extends the json.JSONEncoder class so that we can use a lot of the
# built-in json support.


def _get_addr_str(reg_type: str, address: int) -> str:
    fmt = {
        "SCOM": "0x{:08X}",  # 4-byte address
        "IDSCOM": "0x{:016X}",  # 8-byte address
    }

    return fmt[reg_type].format(address)  # throws exception if key not found


def _get_value_str(reg_type: str, value: int) -> str:
    fmt = {
        "SCOM": "0x{:016X}",  # 8-byte value
        "IDSCOM": "0x{:016X}",  # 8-byte value
    }

    return fmt[reg_type].format(value)


class _ChipDataEncoder(json.JSONEncoder):
    def default(self, o: object) -> dict:
        if isinstance(o, cd.Base):
            j = {
                "version": o.version,
                "model_ec": o.model_ec,
            }

            if o.registers:
                j["registers"] = o.registers

            if o.isolation_nodes:
                j["isolation_nodes"] = o.isolation_nodes

            if o.root_nodes:
                j["root_nodes"] = o.root_nodes

            if o.capture_groups:
                j["capture_groups"] = o.capture_groups

            return j

        if isinstance(o, cd.Register):
            j = {}

            if "SCOM" != o.reg_type:  # Don't add default to save space.
                j["reg_type"] = o.reg_type

            if "RW" != o.access:  # Don't add default to save space.
                j["access"] = o.access

            j["instances"] = dict(
                zip(
                    o.instances.keys(),
                    map(
                        lambda v: _get_addr_str(o.reg_type, v),
                        o.instances.values(),
                    ),
                )
            )

            return j

        if isinstance(o, cd.IsolationNode):
            j = {}

            if "SCOM" != o.reg_type:  # Don't add default to save space.
                j["reg_type"] = o.reg_type

            j["instances"] = o.instances
            j["rules"] = o.rules
            if o.op_rules:
                j["op_rules"] = o.op_rules
            j["bits"] = o.bits

            if o.capture_groups:
                j["capture_groups"] = o.capture_groups

            return j

        if isinstance(o, cd.IsolationRule):
            return {
                "attn_type": o.attn_type,
                "node_inst": o.node_inst,
                "expr": o.expr,
            }

        if isinstance(o, cd.ExprReg):
            j = {
                "expr_type": o.expr_type,
                "reg_name": o.reg_name,
            }

            if o.reg_inst:
                j["reg_inst"]: o.reg_inst

            return j

        if isinstance(o, cd.ExprInt):
            return {
                "expr_type": o.expr_type,
                "int_value": _get_value_str(o.reg_type, o.int_value),
            }

        if isinstance(o, cd.ExprAnd):
            return {
                "expr_type": o.expr_type,
                "exprs": o.exprs,
            }

        if isinstance(o, cd.ExprOr):
            return {
                "expr_type": o.expr_type,
                "exprs": o.exprs,
            }

        if isinstance(o, cd.ExprNot):
            return {
                "expr_type": o.expr_type,
                "expr": o.expr,
            }

        if isinstance(o, cd.ExprLeftShift):
            return {
                "expr_type": o.expr_type,
                "expr": o.expr,
                "shift_value": o.shift_value,
            }

        if isinstance(o, cd.ExprRightShift):
            return {
                "expr_type": o.expr_type,
                "expr": o.expr,
                "shift_value": o.shift_value,
            }

        if isinstance(o, cd.IsolationBit):
            j = {"desc": o.desc}

            if o.child_node:
                j["child_node"] = {"name": o.child_node["name"]}

                if o.child_node["inst"]:
                    j["child_node"]["inst"] = o.child_node["inst"]

            if o.capture_groups:
                j["capture_groups"] = o.capture_groups

            return j

        if isinstance(o, cd.IsolationWriteOps):
            j = {"op_rule": o.op_rule, "reg_name": o.reg_name}
            return j

        if isinstance(o, cd.RootNode):
            return {
                "name": o.name,
                "inst": o.inst,
            }

        if isinstance(o, cd.CaptureGroup):
            return {
                "group_name": o.group_name,
                "group_inst": o.group_inst,
            }

        if isinstance(o, cd.CaptureRegister):
            return {
                "reg_name": o.reg_name,
                "reg_inst": o.reg_inst,
            }

        # Call the default method for other types
        return json.JSONEncoder.default(self, o)


# Decoder support --------------------------------------------------------------
# Unfortunately, we cannot extent the json.JSONDecoder like we did with the
# encoder because when using a custom encoder it is applied to all JSON objects
# in the string/file. Since the Chip Data JSON has a complex design with many
# different JSON object formats, we can't use this approach without adding a
# property to each object indicating what type of object it is. That would end
# up bloating the JSON and is not desired. Instead, we'll create a series of
# decoder functions. Then the decoder helper at the bottom of the file will use
# the default decoder and apply these decoder functions on the returned
# dictionary. This is not as efficient, but will be the approach for now.


def _decodeInstanceMap(d: dict) -> dict:
    # Need to convert the keys to integers.
    return dict(zip(map(lambda k: int(k), d.keys()), d.values()))


def _decodeRegister(d: dict) -> cd.Register:
    reg_type = d["reg_type"] if "reg_type" in d else "SCOM"
    access = d["access"] if "access" in d else "RW"

    register = cd.Register(reg_type, access)

    for k, v in d["instances"].items():
        register.addInstance(int(k), int(v, 16))

    return register


def _decodeRuleExpression(reg_type: str, d: dict) -> object:
    expr_type = d["expr_type"]

    if "reg" == expr_type:
        reg_inst = _decodeInstanceMap(d["reg_inst"]) if "reg_inst" in d else {}
        return cd.ExprReg(d["reg_name"], reg_inst)

    if "int" == expr_type:
        return cd.ExprInt(reg_type, int(d["int_value"], 16))

    if "and" == expr_type:
        return cd.ExprAnd(
            list(map(lambda e: _decodeRuleExpression(reg_type, e), d["exprs"]))
        )

    if "or" == expr_type:
        return cd.ExprOr(
            list(map(lambda e: _decodeRuleExpression(reg_type, e), d["exprs"]))
        )

    if "not" == expr_type:
        return cd.ExprNot(_decodeRuleExpression(reg_type, d["expr"]))

    if "lshift" == expr_type:
        return cd.ExprLeftShift(
            _decodeRuleExpression(reg_type, d["expr"]), d["shift_value"]
        )

    if "rshift" == expr_type:
        return cd.ExprRightShift(
            _decodeRuleExpression(reg_type, d["expr"]), d["shift_value"]
        )


def _decodeIsolationRule(reg_type: str, d: dict) -> cd.IsolationRule:
    return cd.IsolationRule(
        d["attn_type"],
        d["node_inst"],
        _decodeRuleExpression(reg_type, d["expr"]),
    )


def _decodeCaptureGroup(d: dict) -> cd.CaptureGroup:
    return cd.CaptureGroup(
        d["group_name"], _decodeInstanceMap(d["group_inst"])
    )


def _decodeIsolationBit(d: dict) -> cd.IsolationBit:
    bit = cd.IsolationBit(d["desc"])

    if "child_node" in d:
        if "inst" in d["child_node"]:
            inst = _decodeInstanceMap(d["child_node"]["inst"])
        else:
            inst = {}

        bit.addChildNode(d["child_node"]["name"], inst)

    if "capture_groups" in d:
        for e in d["capture_groups"]:
            bit.addCaptureGroup(_decodeCaptureGroup(e))

    return bit


def _decodeWriteOp(d: dict) -> cd.IsolationWriteOps:
    return cd.IsolationWriteOps(d["op_rule"], d["reg_name"])


def _decodeIsolationNode(d: dict) -> cd.IsolationNode:
    reg_type = d["reg_type"] if "reg_type" in d else "SCOM"

    node = cd.IsolationNode(reg_type)

    # Don't need to parse `instances` because that will be recreated in the
    # `addRule()` function.

    for e in d["rules"]:
        node.addRule(_decodeIsolationRule(reg_type, e))

    for k, v in d["bits"].items():
        node.addBit(k, _decodeIsolationBit(v))

    if "capture_groups" in d:
        for e in d["capture_groups"]:
            node.addCaptureGroup(_decodeCaptureGroup(e))

    if "op_rules" in d:
        for k, v in d["op_rules"].items():
            node.addWriteOp(k, _decodeWriteOp(v))

    return node


def _decodeRootNode(d: dict) -> cd.RootNode:
    return cd.RootNode(d["name"], d["inst"])


def _decodeCaptureRegister(d: dict) -> cd.CaptureRegister:
    return cd.CaptureRegister(d["reg_name"], _decodeInstanceMap(d["reg_inst"]))


def _decodeBase(d: dict) -> cd.Base:
    base = cd.Base(d["version"], d["model_ec"])

    if "registers" in d:
        for k, v in d["registers"].items():
            base.addRegister(k, _decodeRegister(v))

    if "isolation_nodes" in d:
        for k, v in d["isolation_nodes"].items():
            base.addIsolationNode(k, _decodeIsolationNode(v))

    if "root_nodes" in d:
        for k, v in d["root_nodes"].items():
            base.addRootNode(k, _decodeRootNode(v))

    if "capture_groups" in d:
        for k, v in d["capture_groups"].items():
            for e in v:
                base.addCaptureRegister(k, _decodeCaptureRegister(e))

    return base


# ------------------------------------------------------------------------------


def json_encode(obj: object, fp: object, indent=None):
    json.dump(obj, fp, cls=_ChipDataEncoder, indent=indent)
    fp.write("\n")


def json_encodes(obj: object, indent=None) -> str:
    return json.dump(obj, cls=_ChipDataEncoder, indent=indent)


def json_decode(fp: object) -> object:
    # See note above for why we didn't extend the json.JSONDecoder class
    return _decodeBase(json.load(fp))


def json_decodes(s: str) -> object:
    # See note above for why we didn't extend the json.JSONDecoder class
    return _decodeBase(json.loads(s))
