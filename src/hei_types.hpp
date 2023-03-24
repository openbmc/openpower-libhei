/**
 * @file hei_types.hpp
 *
 * This file contains simple types/enums used throughout all of the isolator
 * code.
 */

#pragma once

#include <stdint.h>

#include <limits>

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
 *   This is defined as a 4-byte field in the Chip Data Files.
 */
using ChipType_t = uint32_t;

/**
 * Each isolation node within a chip must have a unique ID. These IDs (combined
 * with other information) will be passed back to the user application to
 * identify all of the active errors reported by this chip. Note that some
 * isolation nodes will have multiple instances within a chip. An ID will be
 * used for all instances of a node. See enum Instance_t for details on the
 * instance value.
 *
 * Values:
 *   The isolator does not need to know the possible values because the values
 *   are passed from the Chip Data Files to the user application.
 *
 * Range:
 *   This is a 2-byte field, which should be sufficient to support all the
 *   isolation nodes on a typical chip.
 */
using NodeId_t = uint16_t;

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
 *   This is defined as the minimum 1-byte field in the Chip Data Files.
 */
enum RegisterType_t : uint8_t
{
    REG_TYPE_SCOM    = 0x01, ///< Power Systems SCOM register.
    REG_TYPE_ID_SCOM = 0x02, ///< Power Systems Indirect SCOM register.
};

/**
 * Each register within a chip must have a unique ID. These IDs (combined with
 * other information) will be passed back to the user application to identify
 * register contents captured for debugging purposes. Note that some registers
 * will have multiple instances within a chip. An ID will be used for all
 * instances of a register. See Instance_t for details on the register instance
 * value.
 *
 * Values:
 *   The isolator does not need to know the possible values because the values
 *   are passed from the Chip Data Files to the user application.
 *
 * Range:
 *   This is defined as a 3-byte field in the Chip Data Files, which should be
 *   sufficient to support all the registers on a typical chip.
 */
// IMPORTANT: typedef or using only creates an alias which is not a new type.
// Need to defined this as an enum so that template specialization will detect a
// new variable type. See note aboved for details.
enum RegisterId_t : uint32_t;

/**
 * A chip could contain more than one instance of a register or node. For
 * example, a register could exist for each instance of a core on a processor
 * chip. This field will be used to differeniate multiple instances of a
 * register in order to avoid repeating common information for every instance.
 *
 * Values:
 *   Not all registers or nodes will have multiple instances. So the default
 *   instance value is 0, which always indicates the first (or only) logical
 *   instance. Then a value of 1-255 can be used for each subsequent instance.
 *
 * Range:
 *   This is defined as a 1-byte field in the Chip Data Files.
 */
using Instance_t = uint8_t;

/**
 * This is used to defined a bit field for a register or node.
 *
 * Values:
 *   The widest supported register type is only 64-bits (value 0-63).
 *
 * Range:
 *   This is defined as a 1-byte field in the Chip Data Files.
 */
using BitPosition_t = uint8_t;

/**
 * The maximum bit position supported by the BitPosition_t type. Note that this
 * value does not represent the maximum bit position for a specific register
 * type. That will need to be calculated seperately.
 */
constexpr auto MAX_BIT_POSITION = std::numeric_limits<BitPosition_t>::max();

/**
 * The hardware address of a register (right justified).
 *
 * Values:
 *   Currently only supporting 1, 2, 4, or 8 byte addresses.
 *
 * Range:
 *   The maximum supported address requires an 8-byte field.
 */
using RegisterAddress_t = uint64_t;

/**
 * The hardware register attribute flags.
 *
 * Values:
 *   Each bit within this field represents an attribute flag. If the bit is 0,
 *   the flag is disabled. If the bit is 1, the flag is enabled.
 *
 * Range:
 *   This is defined as a 1-byte field in the Chip Data Files.
 */
enum RegisterAttributeFlags_t : uint8_t
{
    REG_ATTR_ACCESS_READ  = 0x80, ///< Register read access access
    REG_ATTR_ACCESS_WRITE = 0x40, ///< Register write access access
    REG_ATTR_RESERVED     = 0x3f, ///< Reserved/unused bits
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
 *   This is defined as a 1-byte field in the Chip Data Files.
 */
enum AttentionType_t : uint8_t
{
    // clang-format off

    /** System checkstop hardware attention. Unrecoverable, fatal error. */
    // TODO: This is deprecated and will be removed. Use CHIP_CS instead.
    ATTN_TYPE_CHECKSTOP   = 1,

    /**
     * Hardware error event indicating that an entire chip has checkstopped and
     * is no longer usable.
     */
    ATTN_TYPE_CHIP_CS     = 1,

    /**
     * Hardware error event indicating that a unit within a chip has
     * checkstopped and is no longer usable. Other units within the chip should
     * continue uninterrupted. Possible degraded functionality.
     */
    ATTN_TYPE_UNIT_CS     = 2,

    /**
     * Hardware error event in which the hardware recovered. The system should
     * be able to continue uninterrupted. Possible degraded functionality.
     */
    ATTN_TYPE_RECOVERABLE = 3,

    /**
     * Software or hardware event requiring action by the service processor
     * firmware.
     */
    ATTN_TYPE_SP_ATTN     = 4,

    /**
     * Software or hardware event requiring action by the host firmware.
     */
    ATTN_TYPE_HOST_ATTN   = 5,

    // clang-format on
};

/**
 * Each Chip Data File will begin with a keyword signafying it is a Chip Data
 * File.
 */
using FileKeyword_t = uint64_t;

constexpr FileKeyword_t KW_CHIPDATA = 0x4348495044415441; // "CHIPDATA" ASCII

} // end namespace libhei
