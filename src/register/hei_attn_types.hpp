#pragma once

/**
@file  hei_attn_types.hpp
@brief Specification of attention types.
@note  The highest priority attention types should have a lower value.
*/

#include <stdint.h>

namespace libhei
{

typedef uint8_t ATTENTION_TYPE;

enum ATTENTION_VALUE_TYPE
{
    INVALID_ATTENTION_TYPE  = 0,
    MACHINE_CHECK           = 1,
    CHECK_STOP              = MACHINE_CHECK,
    UNIT_CS                 = 2,
    PROC_CS                 = UNIT_CS,
    RECOVERABLE             = 3,
    SPECIAL                 = 4,
    HOST_ATTN               = 5,
    END_ATTENTION_TYPE      = 6
};

}
//end namespace libhei
