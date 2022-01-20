#include "simulator.hpp"

START_TEST_CASE(tod_fault)

CHIP(proc0, P10_20)

START_ITERATION

REG_SCOM(proc0, 0x570F001C, 0x4000000000000000) // GLOBAL_CS_FIR[1]
REG_SCOM(proc0, 0x570F001B, 0x4000000000000000) // GLOBAL_RE_FIR[1]
REG_SCOM(proc0, 0x01040000, 0x0800000000000000) // TP_CHIPLET_CS_FIR[4]
REG_SCOM(proc0, 0x01040001, 0x0800000000000000) // TP_CHIPLET_RE_FIR[4]
REG_SCOM(proc0, 0x01040100, 0x0000007000000000) // TP_LOCAL_FIR[25:27]
REG_SCOM(proc0, 0x01040103, 0x0000001000000000) // TP_LOCAL_FIR_MASK
REG_SCOM(proc0, 0x01040107, 0x0000002000000000) // TP_LOCAL_FIR_ACT1

REG_SCOM(proc0, 0x00040030, 0xf000000000000000) // TOD_ERROR
REG_SCOM(proc0, 0x00040032, 0xc000000000000000) // TOD_ERROR_MASK
REG_SCOM(proc0, 0x00040033, 0xa000000000000000) // TOD_ERROR_ROUTING

EXP_SIG(proc0, 0x7d9b, 0, 3, CHECKSTOP)
EXP_SIG(proc0, 0xdf2a, 0, 26, RECOVERABLE)

END_ITERATION

END_TEST_CASE
