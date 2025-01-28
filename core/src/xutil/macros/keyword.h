#pragma once

#include "xutil/prefix.h"

// the register keyword will be deprecated in C++
#ifndef __cplusplus
#    define __xu_register__ register
#else
#    define __xu_register__
#endif
#define __xu_volatile__ volatile

#if defined(XU_COMPILER_IS_GCC)
#    define __xu_func__ __FUNCTION__
#    define __xu_file__ __FILE__
#    define __xu_line__ __LINE__
#    define __xu_date__ __DATE__
#    define __xu_time__ __TIME__
#    define __xu_asm__ __asm__
#    define __xu_inline__ __inline__
#    define __xu_inline_force__ __inline__ __attribute__((always_inline))
#    define __xu_packed__ __attribute__((packed, aligned(1)))
#    define __xu_aligned__(a) __attribute__((aligned(a)))
// gcc will generate attribute ignored warning
#    if defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_IA64) || defined(_M_X64)
#        define __xu_cdecl__
#        define __xu_stdcall__
#        define __xu_fastcall__
#        define __xu_thiscall__
#    else
#        define __xu_cdecl__ __attribute__((__cdecl__))
#        define __xu_stdcall__ __attribute__((__stdcall__))
#        define __xu_fastcall__ __attribute__((__fastcall__))
#        define __xu_thiscall__ __attribute__((__thiscall__))
#    endif
#elif defined(XU_COMPILER_IS_TINYC)
#    define __xu_func__ __func__
#    define __xu_file__ __FILE__
#    define __xu_line__ __LINE__
#    define __xu_date__ __DATE__
#    define __xu_time__ __TIME__
#    define __xu_asm__ __asm__
#    define __xu_inline__ __inline__
#    define __xu_inline_force__ __inline__ __attribute__((always_inline))
#    define __xu_packed__ __attribute__((packed, aligned(1)))
#    define __xu_aligned__(a) __attribute__((aligned(a)))
#    define __xu_cdecl__ __attribute__((cdecl))
#    define __xu_stdcall__ __attribute__((stdcall))
#    define __xu_fastcall__ __attribute__((fastcall))
#    define __xu_thiscall__ __attribute__((thiscall))
#else
#    define __xu_asm__
#    define __xu_inline__ inline
#    define __xu_inline_force__ inline
#    define __xu_func__ ""
#    define __xu_file__ ""
#    define __xu_date__ ""
#    define __xu_time__ ""
#    define __xu_line__ (0)
#    define __xu_cdecl__
#    define __xu_stdcall__
#    define __xu_fastcall__
#    define __xu_thiscall__
#    define __xu_packed__
#    define __xu_aligned__(a)
#endif

#if defined(XU_COMPILER_IS_GCC)
#    define __xu_inline__ __inline__
#    define __xu_inline_force__ __inline__ __attribute__((always_inline))
#elif defined(XU_COMPILER_IS_TINYC)
#    define __xu_inline__ __inline__
#    define __xu_inline_force__ __inline__ __attribute__((always_inline))
#else
#    define __xu_inline__ inline
#    define __xu_inline_force__ inline
#endif

/*! @def __xu_cpu_aligned__
 *
 * the cpu byte alignment
 */
#if (XU_CPU_BITBYTE == 8)
#    define __xu_cpu_aligned__ __xu_aligned__(8)
#elif (XU_CPU_BITBYTE == 4)
#    define __xu_cpu_aligned__ __xu_aligned__(4)
#elif (XU_CPU_BITBYTE == 2)
#    define __xu_cpu_aligned__ __xu_aligned__(2)
#else
#    error unknown cpu bytes
#endif

// like
#if defined(XU_COMPILER_IS_GCC) && XU_COMPILER_VERSION_BT(2, 0)
#    define __xu_likely__(x) __builtin_expect((x), 1)
#    define __xu_unlikely__(x) __builtin_expect((x), 0)
#else
#    define __xu_likely__(x) (x)
#    define __xu_unlikely__(x) (x)
#endif

// debug
#ifdef __xu_debug__
#    define __xu_debug_decl__ , xu_char_t const *func_, xu_size_t line_, xu_char_t const *file_
#    define __xu_debug_vals__ , __xu_func__, __xu_line__, __xu_file__
#    define __xu_debug_args__ , func_, line_, file_
#else
#    define __xu_debug_decl__
#    define __xu_debug_vals__
#    define __xu_debug_args__
#endif

/* *******************************************************
 *  newline
 */
#define __xu_newline__ "\n"

/* *******************************************************
 *  has feature
 */
#ifdef __has_feature
#    define __xu_has_feature__(x) __has_feature(x)
#else
#    define __xu_has_feature__(x) 0
#endif

/* *******************************************************
 *  has include
 */
#ifdef __has_include
#    define __xu_has_include__(x) __has_include(x)
#else
#    define __xu_has_include__(x) 0
#endif

/* *******************************************************
 *  has builtin
 */
#ifdef __has_builtin
#    define __xu_has_builtin__(x) __has_builtin(x)
#else
#    define __xu_has_builtin__(x) 0
#endif

/* *******************************************************
 *  no_sanitize_address
 */
#if __xu_has_feature__(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#    define __xu_no_sanitize_address__ __attribute__((no_sanitize_address))
#else
#    define __xu_no_sanitize_address__
#endif

/* *******************************************************
 *  thread_local
 */
#if __xu_has_feature__(c_thread_local)
#    define __xu_thread_local__ _Thread_local
#elif defined(XU_CONFIG_KEYWORD_HAVE_Thread_local)
#    define __xu_thread_local__ _Thread_local
#elif defined(XU_CONFIG_KEYWORD_HAVE__thread)
#    define __xu_thread_local__ __thread
#elif defined(XU_COMPILER_IS_MSVC) || defined(XU_COMPILER_IS_BORLAND)
#    define __xu_thread_local__ __declspec(thread)
#endif

/*! the type reference keyword for defining xu_xxxx_ref_t
 *
 * typedef __xu_typeref__(xxxx);
 *
 *
 * suppress gcc 4.9 on c++ codes warning: '__xu_yyyy_t' has a field '__xu_yyyy_t::xxxx' whose type uses the anonymous
 namespace
 *
 * @code
 *
   typedef struct{}*    xu_xxxx_ref_t;

   typedef struct __xu_yyyy_t
   {
       xu_xxxx_ref_t    xxxx;

   }__xu_yyyy_t;

 *
 *
 * @endcode
 *
 */
#define __xu_typeref__(object)                                                                                         \
    struct __xu_##object##_dummy_t                                                                                     \
    {                                                                                                                  \
        xu_int_t dummy;                                                                                                \
    } const* xu_##object##_ref_t

// macros
#define __xu_mstring__(x) #x
#define __xu_mstring_ex__(x) __xu_mstring__(x)

#define __xu_mconcat__(a, b) a##b
#define __xu_mconcat_ex__(a, b) __xu_mconcat__(a, b)

#define __xu_mconcat3__(a, b, c) a##b##c
#define __xu_mconcat3_ex__(a, b, c) __xu_mconcat3__(a, b, c)

#define __xu_mconcat4__(a, b, c, d) a##b##c##d
#define __xu_mconcat4_ex__(a, b, c, d) __xu_mconcat4__(a, b, c, d)

#define __xu_mconcat5__(a, b, c, d, e) a##b##c##d##e
#define __xu_mconcat5_ex__(a, b, c, d, e) __xu_mconcat5__(a, b, c, d, e)

#define __xu_mconcat6__(a, b, c, d, e, f) a##b##c##d##e##f
#define __xu_mconcat6_ex__(a, b, c, d, e, f) __xu_mconcat6__(a, b, c, d, e, f)

#define __xu_mconcat7__(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define __xu_mconcat7_ex__(a, b, c, d, e, f, g) __xu_mconcat7__(a, b, c, d, e, f, g)

#define __xu_mconcat8__(a, b, c, d, e, f, g, h) a##b##c##d##e##f##g##h
#define __xu_mconcat8_ex__(a, b, c, d, e, f, g, h) __xu_mconcat8__(a, b, c, d, e, f, g, h)

#define __xu_mconcat9__(a, b, c, d, e, f, g, h, i) a##b##c##d##e##f##g##h##i
#define __xu_mconcat9_ex__(a, b, c, d, e, f, g, h, i) __xu_mconcat9__(a, b, c, d, e, f, g, h, i)

#define __xu_mstrcat__(a, b) a b
#define __xu_mstrcat3__(a, b, c) a b c
#define __xu_mstrcat4__(a, b, c, d) a b c d
#define __xu_mstrcat5__(a, b, c, d, e) a b c d e
#define __xu_mstrcat6__(a, b, c, d, e, f) a b c d e f
#define __xu_mstrcat7__(a, b, c, d, e, f, g) a b c d e f g
#define __xu_mstrcat8__(a, b, c, d, e, f, g, h) a b c d e f g h
#define __xu_mstrcat9__(a, b, c, d, e, f, g, h, i) a b c d e f g h i
