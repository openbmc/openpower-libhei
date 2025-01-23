#pragma once

// The purpose of this file is to include common headers that will be used
// internally throughout this library. It is advised not to include this in any
// headers that are installed externally to avoid ballooning the number of
// headers that are externally exposed.

// Standard library includes
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

// Library includes
#include <hei_chip.hpp>
#include <hei_types.hpp>
#include <hei_user_interface.hpp>

// Common macros used throughout this library
// Only defined if not already defined so that the user application can
// supersede these definitions with their own if so desired.
#ifndef HEI_ASSERT
#define HEI_ASSERT(expression) assert(expression)
#endif

#ifndef HEI_ERR
#define HEI_ERR(...)                                                           \
    {                                                                          \
        libhei::hei_err((char*)__VA_ARGS__);                                   \
    }
#endif

#ifndef HEI_INF
#define HEI_INF(...)                                                           \
    {                                                                          \
        libhei::hei_inf((char*)__VA_ARGS__);                                   \
    }
#endif
