/**
 * @file hei_types.hpp
 *
 * This file contains simple types/enums used throughout all of the isolator
 * code.
 */

#pragma once

#include <stdint.h>

namespace libhei
{

/**
 * Each Chip Data File will contain a 32-bit number representing the type of
 * chip characterized by the file. That value must also be provide by the user
 * application for each chip used in isolation so the isolator will know which
 * Chip Data File to reference.
 */
typedef uint32_t ChipType_t;

/**
 * This is used for error checking. If a chip type contains this value, it is
 * not a valid chip type.
 */
static constexpr ChipType_t DEFAULT_CHIP_TYPE = 0;

/**
 * User application uses this to know how to access a particular register.
 */
typedef uint16_t RegisterType_t;

/**
 * Access level for a register.
 */
typedef enum
{
    ACCESS_NONE = 0x0, //< No access
    ACCESS_RO   = 0x1, //< Read-only access
    ACCESS_WO   = 0x2, //< Write-only access
    ACCESS_RW   = 0x3, //< Read/Write access
} AccessLevel_t;

} // end namespace libhei
