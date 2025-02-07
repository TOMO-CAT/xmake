#pragma once

#include "xutil/config.h"

/* *******************************************************
 *  tiny c
 */
#if defined(__TINYC__)
#    define XU_COMPILER_IS_TINYC
#    define XU_COMPILER_VERSION_BT(major, minor) (__TINYC__ > ((major)*100 + (minor)))
#    define XU_COMPILER_VERSION_BE(major, minor) (__TINYC__ >= ((major)*100 + (minor)))
#    define XU_COMPILER_VERSION_EQ(major, minor) (__TINYC__ == ((major)*100 + (minor)))
#    define XU_COMPILER_VERSION_LT(major, minor) (__TINYC__ < ((major)*100 + (minor)))
#    define XU_COMPILER_VERSION_LE(major, minor) (__TINYC__ <= ((major)*100 + (minor)))
#    define XU_COMPILER_STRING "tiny c"
#    if (__TINYC__ == 926)
#        define XU_COMPILER_VERSION_STRING "tiny c 0.9.26"
#    elif (__TINYC__ == 927)
#        define XU_COMPILER_VERSION_STRING "tiny c 0.9.27"
#    else
#        error Unknown Tiny C Compiler Version
#    endif

/* *******************************************************
 *  gcc or clang
 */
#elif defined(__GNUC__) || defined(__clang__)
#    if defined(__unix__) || defined(__unix) || defined(unix)
#        define XU_COMPILER_LIKE_UNIX
#    endif
#    if defined(__clang__)
#        define XU_COMPILER_IS_CLANG
#    endif

// we always need to be compatible with gcc style code if the current compiler is clang
#    define XU_COMPILER_IS_GCC

//========================== clang
#    if defined(__clang__)
#        undef XU_COMPILER_STRING
#        define XU_COMPILER_STRING "clang c/c++"
#        define XU_COMPILER_VERSION_BT(major, minor)                                                                   \
            ((__clang_major__ * 100 + __clang_minor__) > ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_BE(major, minor)                                                                   \
            ((__clang_major__ * 100 + __clang_minor__) >= ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_EQ(major, minor)                                                                   \
            ((__clang_major__ * 100 + __clang_minor__) == ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_LT(major, minor)                                                                   \
            ((__clang_major__ * 100 + __clang_minor__) < ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_LE(major, minor)                                                                   \
            ((__clang_major__ * 100 + __clang_minor__) <= ((major)*100 + (minor)))
#        if defined(__VERSION__)
#            undef XU_COMPILER_VERSION_STRING
#            define XU_COMPILER_VERSION_STRING __VERSION__
#        elif defined(__clang_version__)
#            undef XU_COMPILER_VERSION_STRING
#            define XU_COMPILER_VERSION_STRING __clang_version__
#        endif

//========================== gcc
#    elif defined(__GNUC__)
#        define XU_COMPILER_VERSION_BT(major, minor) ((__GNUC__ * 100 + __GNUC_MINOR__) > ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_BE(major, minor) ((__GNUC__ * 100 + __GNUC_MINOR__) >= ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_EQ(major, minor) ((__GNUC__ * 100 + __GNUC_MINOR__) == ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_LT(major, minor) ((__GNUC__ * 100 + __GNUC_MINOR__) < ((major)*100 + (minor)))
#        define XU_COMPILER_VERSION_LE(major, minor) ((__GNUC__ * 100 + __GNUC_MINOR__) <= ((major)*100 + (minor)))
#        define XU_COMPILER_STRING "gnu c/c++"
#        if __GNUC__ == 2
#            if __GNUC_MINOR__ < 95
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ <2.95"
#            elif __GNUC_MINOR__ == 95
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ 2.95"
#            elif __GNUC_MINOR__ == 96
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ 2.96"
#            else
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ > 2.96 && < 3.0"
#            endif
#        elif __GNUC__ == 3
#            if __GNUC_MINOR__ == 2
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ 3.2"
#            elif __GNUC_MINOR__ == 3
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ 3.3"
#            elif __GNUC_MINOR__ == 4
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ 3.4"
#            else
#                define XU_COMPILER_VERSION_STRING "gnu c/c++ > 3.4 && < 4.0"
#            endif
#        elif __GNUC__ >= 4 && defined(__GNUC_MINOR__)
#            define XU_COMPILER_VERSION_STRING                                                                         \
                __xu_mstrcat4__("gnu c/c++ ", __xu_mstring_ex__(__GNUC__), ".", __xu_mstring_ex__(__GNUC_MINOR__))
#        else
#            error Unknown gnu c/c++ Compiler Version
#        endif
#    endif

//========================== unknown compiler
#else
#    define XU_COMPILER_STRING "unknown compiler"
#    define XU_COMPILER_VERSION_STRING "unknown compiler version"
#    define XU_COMPILER_IS_UNKNOWN
#endif
