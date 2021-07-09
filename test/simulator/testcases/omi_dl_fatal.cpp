#include <simulator.hpp>

START_TEST_CASE(omi_dl_fatal)

CHIP(proc0, P10_20)
CHIP(ocmb1, EXPLORER_20)

START_ITERATION

REG_SCOM(proc0, 0x570F001B, 0x0008000000000000) // GLOBAL_RE_FIR[12]
REG_SCOM(proc0, 0x0C040001, 0x0004000000000000) // MC_CHIPLET_RE_FIR[13]
REG_SCOM(proc0, 0x0C011400, 0x0000080000000000) // MC_OMI_DL_FIR(0)[20]
REG_SCOM(proc0, 0x0C011407, 0x0000080000000000) // MC_OMI_DL_FIR_ACT1(0)
REG_SCOM(proc0, 0x0C011423, 0x0000000000000001) // OMI_DL_ERROR_HOLD(1)[63]

REG_SCOM(ocmb1, 0x08040000, 0x0008000000000000) // OCMB_CHIPLET_CS_FIR[12]
REG_SCOM(ocmb1, 0x08012800, 0x8000000000000000) // OCMB_OMI_DL_FIR[0]
REG_SCOM(ocmb1, 0x08012813, 0x0000000000000001) // OMI_DL_ERROR_HOLD[63]

EXP_SIG(proc0, 0xa598, 1, 63, RECOVERABLE)
EXP_SIG(ocmb1, 0x0d6f, 0, 63, UNIT_CS)

END_ITERATION

END_TEST_CASE
