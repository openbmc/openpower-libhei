import copy
import re


def _check_version(version: int) -> int:
    assert 1 <= version and version <= 1, "Unsupported format version: " + str(
        version
    )

    return version


def _check_model_ec(model_ec: list) -> list:
    supported = [
        "P10_10",
        "P10_20",
        "EXPLORER_11",
        "EXPLORER_20",
        "ODYSSEY_10",
    ]

    for v in model_ec:
        assert v in supported, "Unsupported model/EC level: " + model_ec

    return model_ec


def _check_access(access: str) -> str:
    supported = ["RW", "RO", "WO"]
    assert access in supported, "Unsupported access type: " + access

    return access


def _check_attn_type(attn_type: [str, list]) -> [str, list]:
    supported = ["CHIP_CS", "UNIT_CS", "RECOV", "SP_ATTN", "HOST_ATTN"]

    t = type(attn_type)

    if t is str:
        assert attn_type in supported, (
            "Unsupported attention type: " + attn_type
        )

    elif t is list:
        for v in attn_type:
            _check_attn_type(v)

    else:
        raise ValueError("Invalid object type: " + t)

    return attn_type


def _check_reg_type(reg_type: str) -> str:
    supported = ["SCOM", "IDSCOM"]
    assert reg_type in supported, "Unsupported register type: " + reg_type

    return reg_type


def _check_expr_type(expr_type: str) -> str:
    supported = ["reg", "int", "and", "or", "not", "lshift", "rshift"]
    assert expr_type in supported, "Unsupported expression type: " + expr_type

    return expr_type


def _check_name(name: str) -> str:
    assert re.fullmatch("\\w+", name), "Invalid name value: " + name

    return name


def _check_instance(instance: [str, list, dict]) -> [str, list, dict]:
    t = type(instance)

    if t is int:
        assert 0 <= instance, "Invalid instance value: " + str(instance)

    elif t is list:
        for v in instance:
            _check_instance(v)

    elif t is dict:
        for k, v in instance.items():
            _check_instance(k)
            _check_instance(v)

    else:
        raise ValueError("Invalid object type: " + t)

    return instance


def _check_integer(integer: int) -> int:
    assert 0 <= integer, "Invalid integer value: " + str(integer)

    return integer


def _get_range(r: str) -> list:
    assert re.fullmatch("[0-9]+(:[0-9]+)?", r), "Invalid range: " + r

    s = [int(i) for i in r.split(":")]
    if 2 == len(s):
        s = list(range(s[0], s[1] + 1))

    return s


class Register:
    def __init__(self, reg_type: str = "SCOM", access: str = "RW"):
        self.reg_type = _check_reg_type(reg_type)
        self.access = _check_access(access)
        self.instances = {}

    def addInstance(self, instance: int, address: int):
        assert (
            _check_instance(instance) not in self.instances
        ), "Duplicate instance added to register: " + str(instance)

        self.instances[instance] = _check_integer(address)


class RuleExpression:
    def __init__(self, expr_type: str):
        self.expr_type = _check_expr_type(expr_type)


class ExprReg(RuleExpression):
    def __init__(self, reg_name: str, reg_inst: dict = {}):
        super().__init__("reg")
        self.reg_name = _check_name(reg_name)
        self.reg_inst = _check_instance(reg_inst)


class ExprInt(RuleExpression):
    def __init__(self, reg_type: str, int_value: int):
        super().__init__("int")
        self.reg_type = _check_reg_type(reg_type)
        self.int_value = _check_integer(int_value)


class ExprAnd(RuleExpression):
    def __init__(self, exprs: list):
        super().__init__("and")
        self.exprs = exprs


class ExprOr(RuleExpression):
    def __init__(self, exprs: list):
        super().__init__("or")
        self.exprs = exprs


class ExprNot(RuleExpression):
    def __init__(self, expr: object):
        super().__init__("not")
        self.expr = expr


class ExprLeftShift(RuleExpression):
    def __init__(self, expr: object, shift_value: int):
        super().__init__("lshift")
        self.expr = expr
        self.shift_value = _check_integer(shift_value)


class ExprRightShift(RuleExpression):
    def __init__(self, expr: object, shift_value: int):
        super().__init__("rshift")
        self.expr = expr
        self.shift_value = _check_integer(shift_value)


class IsolationRule:
    def __init__(self, attn_type: list, node_inst: list, expr: object):
        self.attn_type = _check_attn_type(attn_type)
        self.node_inst = _check_instance(node_inst)
        self.expr = expr


class CaptureGroup:
    def __init__(self, group_name: str, group_inst: dict):
        self.group_name = _check_name(group_name)
        self.group_inst = _check_instance(group_inst)


class IsolationBit:
    def __init__(self, desc: str):
        self.desc = desc
        self.child_node = {}
        self.capture_groups = []

    def addChildNode(self, name: str, inst: dict = {}):
        assert not self.child_node, (
            "Multiple child nodes given: "
            + self.child_node["name"]
            + " and "
            + name
        )

        self.child_node = {"name": _check_name(name)}

        if inst:
            self.child_node["inst"] = _check_instance(inst)

    def addCaptureGroup(self, group: CaptureGroup):
        self.capture_groups.append(group)


class IsolationNode:
    def __init__(self, reg_type: str = "SCOM"):
        self.reg_type = _check_reg_type(reg_type)
        self.instances = []
        self.rules = []
        self.bits = {}
        self.capture_groups = []

    def addRule(self, rule: IsolationRule):
        self.rules.append(rule)

        # Keep a running list of node instance from each rule added to this
        # isolation node.
        self.instances = sorted(
            list(set().union(self.instances, rule.node_inst))
        )

    def addBit(self, position: str, bit: IsolationBit):
        positions = _get_range(position)
        assert not any([p in self.bits.keys() for p in positions]), (
            "Duplicate bit in isolation node: " + position
        )

        for p in positions:
            self.bits[p] = bit

    def addCaptureGroup(self, group: CaptureGroup):
        self.capture_groups.append(group)


class RootNode:
    def __init__(self, name: str, inst: int):
        self.name = _check_name(name)
        self.inst = _check_instance(inst)


class CaptureRegister:
    def __init__(self, reg_name: str, reg_inst: dict):
        self.reg_name = _check_name(reg_name)
        self.reg_inst = _check_instance(reg_inst)


class Base:
    def __init__(self, version: int, model_ec: list):
        self.version = _check_version(version)
        self.model_ec = _check_model_ec(model_ec)
        self.registers = {}
        self.isolation_nodes = {}
        self.root_nodes = {}
        self.capture_groups = {}

    def addRegister(self, reg_name: str, register: Register):
        assert _check_name(reg_name) not in self.registers, (
            "Duplicate register in base: " + reg_name
        )

        self.registers[reg_name] = register

    def addIsolationNode(self, node_name: str, iso_node: IsolationNode):
        assert _check_name(node_name) not in self.isolation_nodes, (
            "Duplicate isolation node in base: " + node_name
        )

        self.isolation_nodes[node_name] = iso_node

    def addRootNode(self, attn_type: str, root_node: RootNode):
        assert _check_attn_type(attn_type) not in self.root_nodes, (
            "Duplicate root node in base: " + attn_type
        )

        self.root_nodes[attn_type] = root_node

    def addCaptureRegister(self, group_name: str, cap_reg: CaptureRegister):
        if _check_name(group_name) not in self.capture_groups:
            self.capture_groups[group_name] = []

        self.capture_groups[group_name].append(cap_reg)

    def split(self) -> list:
        """
        Returns a list of new Base objects for each model/EC defined by this
        Base object. The data is copied. So, this object will not be modified.
        """

        new_bases = list()

        for model_ec in self.model_ec:
            new_base = Base(self.version, [model_ec])

            # autopep8: off
            new_base.registers = copy.deepcopy(self.registers)
            new_base.isolation_nodes = copy.deepcopy(self.isolation_nodes)
            new_base.root_nodes = copy.deepcopy(self.root_nodes)
            new_base.capture_groups = copy.deepcopy(self.capture_groups)
            # autopep8: on

            new_bases.append(new_base)

        return new_bases

    def merge(self, that_base: object) -> None:
        """
        Takes data from the given Base object and merges it into this object.
        """

        assert set(self.model_ec) == set(
            that_base.model_ec
        ), "Cannot merge Base objects with different model_ec values"

        assert (
            self.version == that_base.version
        ), "Cannot merge Base objects with different versions"

        for reg_name, register in that_base.registers.items():
            self.addRegister(reg_name, register)

        for node_name, iso_node in that_base.isolation_nodes.items():
            self.addIsolationNode(node_name, iso_node)

        for attn_type, root_node in that_base.root_nodes.items():
            self.addRootNode(attn_type, root_node)

        for group_name, cap_regs in that_base.capture_groups.items():
            for cap_reg in cap_regs:
                self.addCaptureRegister(group_name, cap_reg)
