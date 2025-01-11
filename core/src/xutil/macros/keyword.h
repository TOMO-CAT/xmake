#pragma once

// the register keyword will be deprecated in C++
#ifndef __cplusplus
#    define __xm_register__ register
#else
#    define __xm_register__
#endif
#define __xm_volatile__ volatile

#if defined(XM_COMPILER_IS_GCC)
#    define __xm_func__ __FUNCTION__
#    define __xm_file__ __FILE__
#    define __xm_line__ __LINE__
#    define __xm_date__ __DATE__
#    define __xm_time__ __TIME__
#    define __xm_asm__ __asm__
#    define __xm_inline__ __inline__
#    define __xm_inline_force__ __inline__ __attribute__((always_inline))
#    define __xm_packed__ __attribute__((packed, aligned(1)))
#    define __xm_aligned__(a) __attribute__((aligned(a)))
// gcc will generate attribute ignored warning
#    if defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_IA64) || defined(_M_X64)
#        define __xm_cdecl__
#        define __xm_stdcall__
#        define __xm_fastcall__
#        define __xm_thiscall__
#    else
#        define __xm_cdecl__ __attribute__((__cdecl__))
#        define __xm_stdcall__ __attribute__((__stdcall__))
#        define __xm_fastcall__ __attribute__((__fastcall__))
#        define __xm_thiscall__ __attribute__((__thiscall__))
#    endif
#elif defined(XM_COMPILER_IS_TINYC)
#    define __xm_func__ __func__
#    define __xm_file__ __FILE__
#    define __xm_line__ __LINE__
#    define __xm_date__ __DATE__
#    define __xm_time__ __TIME__
#    define __xm_asm__ __asm__
#    define __xm_inline__ __inline__
#    define __xm_inline_force__ __inline__ __attribute__((always_inline))
#    define __xm_packed__ __attribute__((packed, aligned(1)))
#    define __xm_aligned__(a) __attribute__((aligned(a)))
#    define __xm_cdecl__ __attribute__((cdecl))
#    define __xm_stdcall__ __attribute__((stdcall))
#    define __xm_fastcall__ __attribute__((fastcall))
#    define __xm_thiscall__ __attribute__((thiscall))
#else
#    define __xm_asm__
#    define __xm_inline__ inline
#    define __xm_inline_force__ inline
#    define __xm_func__ ""
#    define __xm_file__ ""
#    define __xm_date__ ""
#    define __xm_time__ ""
#    define __xm_line__ (0)
#    define __xm_cdecl__
#    define __xm_stdcall__
#    define __xm_fastcall__
#    define __xm_thiscall__
#    define __xm_packed__
#    define __xm_aligned__(a)
#endif

#if defined(XM_COMPILER_IS_GCC)
#    define __xm_inline__ __inline__
#    define __xm_inline_force__ __inline__ __attribute__((always_inline))
#elif defined(XM_COMPILER_IS_TINYC)
#    define __xm_inline__ __inline__
#    define __xm_inline_force__ __inline__ __attribute__((always_inline))
#else
#    define __xm_inline__ inline
#    define __xm_inline_force__ inline
#endif

/* *******************************************************
 *  newline
 * *******************************************************
 */
#define __xm_newline__ "\n"

/* *******************************************************
 *  has feature
 * *******************************************************
 */
#ifdef __has_feature
#    define __xm_has_feature__(x) __has_feature(x)
#else
#    define __xm_has_feature__(x) 0
#endif

/* *******************************************************
 *  has include
 * *******************************************************
 */
#ifdef __has_include
#    define __xm_has_include__(x) __has_include(x)
#else
#    define __xm_has_include__(x) 0
#endif

/* *******************************************************
 *  has builtin
 * *******************************************************
 */
#ifdef __has_builtin
#    define __xm_has_builtin__(x) __has_builtin(x)
#else
#    define __xm_has_builtin__(x) 0
#endif

/* *******************************************************
 *  no_sanitize_address
 * *******************************************************
 */
#if __xm_has_feature__(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#    define __xm_no_sanitize_address__ __attribute__((no_sanitize_address))
#else
#    define __xm_no_sanitize_address__
#endif

/* *******************************************************
 *  thread_local
 * *******************************************************
 */
#if __xm_has_feature__(c_thread_local)
#    define __xm_thread_local__ _Thread_local
#elif defined(XM_CONFIG_KEYWORD_HAVE_Thread_local)
#    define __xm_thread_local__ _Thread_local
#elif defined(XM_CONFIG_KEYWORD_HAVE__thread)
#    define __xm_thread_local__ __thread
#elif defined(XM_COMPILER_IS_MSVC) || defined(XM_COMPILER_IS_BORLAND)
#    define __xm_thread_local__ __declspec(thread)
#endif

/*! the type reference keyword for defining xm_xxxx_ref_t
 *
 * typedef __xm_typeref__(xxxx);
 *
 *
 * suppress gcc 4.9 on c++ codes warning: '__xm_yyyy_t' has a field '__xm_yyyy_t::xxxx' whose type uses the anonymous
 namespace
 *
 * @code
 *
   typedef struct{}*    xm_xxxx_ref_t;

   typedef struct __xm_yyyy_t
   {
       xm_xxxx_ref_t    xxxx;

   }__xm_yyyy_t;

 *
 *
 * @endcode
 *
 */
#define __xm_typeref__(object)                                                                                         \
    struct __xm_##object##_dummy_t                                                                                     \
    {                                                                                                                  \
        xm_int_t dummy;                                                                                                \
    } const* xm_##object##_ref_t
