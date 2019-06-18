
#pragma once

// Some applications use marcos to define their tracing (aka logging). These
// macros are used such that the format strings are replaced with hash values in
// order to save significant code image space. Therefore, instead of delaring an
// API for traces, the user application must provide this header file which will
// define the follow macros:
//      HEI_TRAC(format, args...) // Generic informational trace
//      HEI_ERR( format, args...) // Error path trace
#include <hei_trace.hpp>

namespace HEI
{

/**
 *  @brief Perform a hardware read operation.
 */
void deviceRead();

#ifndef __HEI_READ_ONLY

/**
 *  @brief Perform a hardware write operation.
 */
void deviceWrite();

#endif

} // end namespace HEI

