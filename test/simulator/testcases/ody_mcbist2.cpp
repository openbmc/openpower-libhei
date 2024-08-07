#include "simulator.hpp"

START_TEST_CASE(ody_mcbist2)

CHIP(ocmb0, ODYSSEY_10)

START_ITERATION

REG_SCOM(ocmb0, 0x570F001C, 0x0080000000000000) // GFIR_CS
REG_SCOM(ocmb0, 0x08040000, 0x0200000000000000) // CFIR_MEM_CHIP_CS
REG_SCOM(ocmb0, 0x08011400, 0x2000000000000000) // MCBIST_FIR
REG_SCOM(ocmb0, 0x08011404, 0x2000000000000000) // MCBIST_FIR_CFG_CHIP_CS

EXP_SIG(ocmb0, 0x8d4d, 0, 2, CHIP_CS)

END_ITERATION

END_TEST_CASE