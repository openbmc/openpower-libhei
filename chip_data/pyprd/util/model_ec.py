from collections import namedtuple

ModelEc = namedtuple("ModelEc", "id type desc")

supported = {
    "EXPLORER_11": ModelEc(0x60D20011, "ocmb", "Explorer 1.1"),
    "EXPLORER_20": ModelEc(0x60D20020, "ocmb", "Explorer 2.0"),
    "ODYSSEY_10": ModelEc(0x60C00010, "ocmb", "Odyssey 1.0"),
    "P10_10": ModelEc(0x20DA0010, "proc", "P10 1.0"),
    "P10_20": ModelEc(0x20DA0020, "proc", "P10 2.0"),
    # TODO: Need to get actual values. Check chipids.H in Hostboot.
    "P11S_10": ModelEc(0xB0000010, "hub", "P11 Spinal 1.0"),
    "P11T_10": ModelEc(0xC0000010, "compute", "P11 Tap 1.0"),
}
