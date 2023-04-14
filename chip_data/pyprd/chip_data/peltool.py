import json
from collections import namedtuple

from pyprd.chip_data import chip_data as cd
from pyprd.util.hash import hash_string_format
from pyprd.util.model_ec import supported as supported_model_ec

AttnType = namedtuple("AttnType", "id desc")

_attn_types = {
    "CHIP_CS": AttnType(1, "chip checkstop"),
    "UNIT_CS": AttnType(2, "unit checkstop"),
    "RECOV": AttnType(3, "recoverable"),
    "SP_ATTN": AttnType(4, "special attention"),
    "HOST_ATTN": AttnType(5, "host attention"),
}


def _format_addr(reg_type: str, reg_addr: int) -> str:
    m = {"SCOM": 4, "IDSCOM": 8}
    return "0x{:0{}x}".format(reg_addr, m[reg_type] * 2)


# -----------------------------------------------------------------------------
# Public functions


def peltool_encode(model_ec: str, base: cd.Base, fp: object):
    """
    Pulls the necessary information from the Chip Data object and writes the
    eBMC PEL parser (peltool) JSON data to the given file pointer. Note that
    the Chip Data Base object could describe more than one model/EC, but the
    JSON data only targets one model/EC. Therefore, the user must specify which
    model/EC to target.
    """
    assert model_ec in supported_model_ec
    assert model_ec in base.model_ec

    data = {
        "model_ec": {
            "desc": supported_model_ec[model_ec].desc,
            "id": "{:08x}".format(supported_model_ec[model_ec].id),
            "type": supported_model_ec[model_ec].type,
        },
        "attn_types": {},
        "registers": {},
        "signatures": {},
    }

    for attn_type in base.root_nodes.keys():
        key, desc = _attn_types[attn_type]

        assert key not in data["attn_types"]

        data["attn_types"][key] = desc

    for reg_name, register in base.registers.items():
        key = hash_string_format(3, reg_name)

        assert key not in data["registers"]

        reg = data["registers"][key] = [reg_name, {}]

        for reg_inst, reg_addr in register.instances.items():
            reg[1][reg_inst] = _format_addr(register.reg_type, reg_addr)

    for node_name, iso_node in base.isolation_nodes.items():
        key = hash_string_format(2, node_name)

        assert key not in data["signatures"]

        node = data["signatures"][key] = [node_name, {}]

        for pos, bit in iso_node.bits.items():
            node[1][pos] = bit.desc

    json.dump(data, fp, indent=4, sort_keys=True)
    fp.write("\n")
