/**
 * @file hei_user_interface.hpp
 *
 * The method for a actions like register access will vary per user application.
 * Therefore, the user application must define all of the APIs listed below.
 */

#pragma once

#include <hei_includes.hpp>

namespace libhei
{

/**
 * @brief Performs a register read operation.
 *
 * @param i_chip     This is a pointer to a user application object that
 *                   represents the target chip. It is provided to the isolator
 *                   by the user application via the isolator main APIs. The
 *                   isolator does not know anything about this object or how to
 *                   use it. The user application is responsible for knowing
 *                   what to do with this parameter.
 *
 * @param i_regType  The user application may support different types of
 *                   registers. This value is provided to the isolater by the
 *                   user application via the Chip Data Files. The user
 *                   application is responsible for knowing what to do with this
 *                   parameter.
 *
 * @param i_address  The register address. The values is a 1, 2, 4, or 8 byte
 *                   address (right justified), which is provided to the
 *                   isolater by the user application via the Chip Data Files.
 *
 * @param o_buffer   Allocated memory space for the returned contents of the
 *                   register.
 *
 * @param io_bufSize The input parameter is the maximum size of the allocated
 *                   o_buffer. The return value is the number of bytes actually
 *                   written to the buffer.
 *
 * @return RC_SUCCESS or RC_REG_ACCESS_UNSUPPORTED or RC_REG_ACCESS_FAILURE
 */
ReturnCode registerRead( void * i_chip, uint64_t i_regType, uint64_t i_address,
                         void * o_buffer, size_t & io_bufSize );

#ifndef __HEI_READ_ONLY

/**
 * @brief Performs a register write operation.
 *
 * @param i_chip     This is a pointer to a user application object that
 *                   represents the target chip. It is provided to the isolator
 *                   by the user application via the isolator main APIs. The
 *                   isolator does not know anything about this object or how to
 *                   use it. The user application is responsible for knowing
 *                   what to do with this parameter.
 *
 * @param i_regType  The user application may support different types of
 *                   registers. This value is provided to the isolater by the
 *                   user application via the Chip Data Files. The user
 *                   application is responsible for knowing what to do with this
 *                   parameter.
 *
 * @param i_address  The register address. The values is a 1, 2, 4, or 8 byte
 *                   address (right justified), which is provided to the
 *                   isolater by the user application via the Chip Data Files.
 *
 * @param i_buffer   Allocated memory space containing the register contents to
 *                   write to hardware.
 *
 * @param io_bufSize The input parameter is the maximum size of the allocated
 *                   o_buffer. The return value is the number of bytes actually
 *                   written to the buffer.
 *
 * @return RC_SUCCESS or RC_REG_ACCESS_UNSUPPORTED or RC_REG_ACCESS_FAILURE
 */
ReturnCode registerWrite( void * i_chip, uint64_t i_regType, uint64_t i_address,
                          void * i_buffer, size_t & io_bufSize );

#endif

} // end namespace libhei

