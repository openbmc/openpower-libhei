/**
 * @file  hei_includes.hpp
 * @brief The purpose of this file is to include common headers that will be
 *        used throughout this library.
 */

#pragma once

// Standard library includes
#include <stdlib.h>
#include <stdint.h>

// The user application must define this header file with the following macros:
//
//   Tracing (inputs same as printf() from <cstdio>):
//      HEI_INF( ... ) // Generic informational trace
//      HEI_ERR( ... ) // Error path trace
//
//  Assertion (at a minimum should work like assert() from <cassert>):
//      HEI_ASSERT( expression )
//
#include <hei_user_defines.hpp>

// Isolator internal includes
#include <hei_return_code.hpp>

