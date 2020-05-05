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
 * A value representing the type of chip that is being accessed. A unique value
 * will exist for each Chip Data File. During isolation, the user application
 * will pass these values to the isolator along with pointers to the user
 * application's chip objects. This tells the isolator which Chip Data File to
 * reference for each chip.
 *
 * Values:
 *   The values are determined by the chip manufacturer. The isolator does not
 *   need to know the possible values because the user application controls
 *   both the Chip Data Files and the input into the isolation function.
 *
 * Range:
 *   A 4-byte field should be sufficient.
 */
using ChipType_t = uint32_t;

/**
 * Different chips will contain different types of registers. Also, a single
 * chip may also support multiple types of registers. These enum values are
 * used to communicate to the user application which type of register access is
 * needed.
 *
 * Values:
 *   The supported register types are listed in this enum.
 *
 * Range:
 *   Power Systems only have a couple different types that would be accessed by
 *   the isolator. The minimum 1-byte field should be sufficient.
 */
enum RegisterType_t : uint8_t
{
    REG_TYPE_INVALID = 0, ///< invalid/unsupported type
    REG_TYPE_SCOM    = 1, ///< Power Systems SCOM register.
    REG_TYPE_ID_SCOM = 2, ///< Power Systems Indirect SCOM register.
};

/**
 * Each register within a chip must have a unique ID. These IDs (combined with
 * other information) will be passed back to the user application to identify
 * all of the active errors reported by this chip. Note that some registers will
 * have multiple instances within a chip. An ID will be used for all instances
 * of a register. See enum RegisterInstance_t for details on the register
 * instance value.
 *
 * Values:
 *   The isolator does not need to know the possible values because the values
 *   are passed from the Chip Data Files to the user application. Therefore, no
 *   values will be listed in this enum except for the default invalid type.
 *
 * Range:
 *   A 2-byte field should be sufficient to support up to 65535 registers on a
 *   chip.
 */
enum RegisterId_t : uint16_t
{
    REG_ID_INVALID = 0, ///< invalid/unsupported type
};

/**
 * A chip could contain more than one instance of a register. For example, a
 * register could exist for each instance of a core on a processor chip.
 * This field will be used to differeniate multiple instances of a register in
 * order to avoid repeating common information for every instance.
 *
 * Values:
 *   Not all registers will have multiple instances. So the default instance
 *   value is 0, which always indicates the first (or only) logical instance.
 *   Then a value of 1-255 can be used for each subsequent instance.
 *
 * Range:
 *   The 1-byte field should be sufficient.
 */
enum RegisterInstance_t : uint8_t
{
    REG_INST_DEFAULT = 0, ///< indicates the first (or only) logical instance
};

/**
 * This is used to defined a bit field for a register. It is mainly used in the
 * Signature class to indicate which bit on a register had an active attention.
 *
 * Values:
 *   The widest supported register is only 64-bits (value 0-63).
 *
 * Range:
 *   Only the minimum 1-byte field is necessary.
 */
typedef uint8_t RegisterBit_t;

/**
 * The hardware address of a register (right justified).
 *
 * Values:
 *   Currently only supporting 1, 2, 4, or 8 byte addresses.
 *
 * Range:
 *   The maximum supported address requires an 8-byte field.
 */
enum RegisterAddress_t : uint64_t
{
    REG_ADDR_INVALID = 0, ///< invalid/unsupported address
};

/**
 * The hardware access level of a register.
 *
 * Values:
 *   The supported access levels are listed in this enum.
 *
 * Range:
 *   Only the minimum 1-byte field is necessary.
 */
enum RegisterAccessLevel_t : uint8_t
{
    REG_ACCESS_NONE = 0x0, ///< No access
    REG_ACCESS_RO   = 0x1, ///< Read-only access
    REG_ACCESS_WO   = 0x2, ///< Write-only access
    REG_ACCESS_RW   = 0x3, ///< Read/Write access
};

/**
 * The Chip Data Files will contain action, rules, etc. based on the supported
 * attention types listed in this enum. The user application must use the
 * values defined in this enum in order to maintain consistency across all
 * chips.
 *
 * Values:
 *   The supported attention types are listed in this enum.
 *
 * Range:
 *   Only the minimum 1-byte field is necessary.
 */
enum AttentionType_t : uint8_t
{
    // clang-format off

    /** System checkstop hardware attention. Unrecoverable, fatal error. */
    ATTN_TYPE_CHECKSTOP   = 1,

    /** Unit checkstop hardware attention. A unit within the system is no longer
     *  usable but the rest of the system should be able to recover. */
    ATTN_TYPE_UNIT_CS     = 2,

    /** Recoverable hardware attention. The system should be able to continue
     *  uninterrupted, possible degraded functionality. */
    ATTN_TYPE_RECOVERABLE = 3,

    /** Software or hardware event requiring action by the service processor
     *  firmware. */
    ATTN_TYPE_SP_ATTN     = 4,

    /** Software or hardware event requiring action by the host firmware. */
    ATTN_TYPE_HOST_ATTN   = 5,

    // clang-format on
};

} // end namespace libhei
