#pragma once

#include <inttypes.h> // for PRIu64

/**
 * @file hei_user_defines.hpp
 *
 * @brief The purpose of this file is to create common defines that
 *        will be used throughout this library.
 *
 * The user application must implement each of the functions listed below:
 *
 *  void libhei::hei_inf(...)
 *  void libhei::hei_err(...)
 *  void libhei::hei_assert(...)
 *
 * See HEI_INF, HEI_ERR and HEI_ASSERT for details.
 *
 * Example implementations
 *
 *  void hei_inf(char* format, ...)
 *  {
 *      va_list args;
 *
 *      printf("I> ");
 *      va_start(args, format);
 *      vprintf(format, args);
 *      va_end(args);
 *      printf("\n");
 *  }
 *
 *  void hei_err(char* format, ...)
 *  {
 *      va_list args;
 *
 *      printf("E> ");
 *      va_start(args, format);
 *      vprintf(format, args);
 *      va_end(args);
 *      printf("\n");
 *  }
 *
 *  void hei_assert(char* condition)
 *  {
 *      printf("Assert! (%s)\n", condition);
 *      assert(0);
 *  }
 */

namespace libhei
{

void hei_inf(char* format, ...); // implemented in user application
void hei_err(char* format, ...); // implemented in user application
void hei_assert(char* msg);      // implemented in user application

} // namespace libhei

/** @brief Common defines used throughout this library */

#define HEI_INF(...)                                                           \
    {                                                                          \
        libhei::hei_inf((char*)__VA_ARGS__);                                   \
    }

#define HEI_ERR(...)                                                           \
    {                                                                          \
        libhei::hei_err((char*)__VA_ARGS__);                                   \
    }

#define HEI_ASSERT(expression)                                                 \
    {                                                                          \
        if (!(expression))                                                     \
        {                                                                      \
            libhei::hei_assert((char*)#expression);                            \
        }                                                                      \
    }
