#pragma once

#include "xutil/prefix.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 *  includes
 */
#include "xutil/macros/type.h"

/* *******************************************************
 *  macros
 */

// the assert backtrace prefix
#define XU_ASSERT_BACKTRACE_PREFIX "    "

// the assert backtrace nframe
#define XU_ASSERT_BACKTRACE_NFRAME (16)

// the assert backtrace dump
#define xu_assert_backtrace_dump() xu_backtrace_dump(XU_ASSERT_BACKTRACE_PREFIX, xu_null, XU_ASSERT_BACKTRACE_NFRAME)

// assert
#ifdef __xu_debug__
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_assertf(x, fmt, arg...)                                                                             \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    xu_abort();                                                                                        \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_and_check_abort(x, fmt, arg...) xu_assertf(x, fmt, ##arg)
#        define xu_assertf_and_check_return(x, fmt, arg...) xu_assertf(x, fmt, ##arg)
#        define xu_assertf_and_check_return_val(x, v, fmt, arg...) xu_assertf(x, fmt, ##arg)
#        define xu_assertf_and_check_goto(x, b, fmt, arg...) xu_assertf(x, fmt, ##arg)
#        define xu_assertf_and_check_break(x, fmt, arg...) xu_assertf(x, fmt, ##arg)
#        define xu_assertf_and_check_continue(x, fmt, arg...) xu_assertf(x, fmt, ##arg)
#    elif defined(XU_COMPILER_IS_MSVC) && XU_COMPILER_VERSION_BE(13, 0)
#        define xu_assertf(x, fmt, ...)                                                                                \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    xu_abort();                                                                                        \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_and_check_abort(x, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#        define xu_assertf_and_check_return(x, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#        define xu_assertf_and_check_return_val(x, v, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#        define xu_assertf_and_check_goto(x, b, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#        define xu_assertf_and_check_break(x, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#        define xu_assertf_and_check_continue(x, fmt, ...) xu_assertf(x, fmt, __VA_ARGS__)
#    else
#        define xu_assertf
#        define xu_assertf_and_check_abort
#        define xu_assertf_and_check_return
#        define xu_assertf_and_check_return_val
#        define xu_assertf_and_check_goto
#        define xu_assertf_and_check_break
#        define xu_assertf_and_check_continue
#    endif
#else
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_assertf(x, fmt, arg...)
#        define xu_assertf_and_check_abort(x, fmt, arg...) xu_check_abort(x)
#        define xu_assertf_and_check_return(x, fmt, arg...) xu_check_return(x)
#        define xu_assertf_and_check_return_val(x, v, fmt, arg...) xu_check_return_val(x, v)
#        define xu_assertf_and_check_goto(x, b, fmt, arg...) xu_check_goto(x, b)
#        define xu_assertf_and_check_break(x, fmt, arg...) xu_check_break(x)
#        define xu_assertf_and_check_continue(x, fmt, arg...) xu_check_continue(x)
#    else
#        define xu_assertf
#        define xu_assertf_and_check_abort
#        define xu_assertf_and_check_return
#        define xu_assertf_and_check_return_val
#        define xu_assertf_and_check_goto
#        define xu_assertf_and_check_break
#        define xu_assertf_and_check_continue
#    endif
#endif

#ifdef __xu_debug__
#    define xu_assert(x) xu_assert_impl(!!(x), #    x __xu_debug_vals__)
#    define xu_assert_and_check_abort(x) xu_assert(x)
#    define xu_assert_and_check_return(x) xu_assert(x)
#    define xu_assert_and_check_return_val(x, v) xu_assert(x)
#    define xu_assert_and_check_goto(x, b) xu_assert(x)
#    define xu_assert_and_check_break(x) xu_assert(x)
#    define xu_assert_and_check_continue(x) xu_assert(x)
#    define xu_assert_and_check_break_state(x, s, v) xu_assert(x)
#else
#    define xu_assert(x) xu_assert_empty_impl()
#    define xu_assert_and_check_abort(x) xu_check_abort(x)
#    define xu_assert_and_check_return(x) xu_check_return(x)
#    define xu_assert_and_check_return_val(x, v) xu_check_return_val(x, v)
#    define xu_assert_and_check_goto(x, b) xu_check_goto(x, b)
#    define xu_assert_and_check_break(x) xu_check_break(x)
#    define xu_assert_and_check_continue(x) xu_check_continue(x)
#    define xu_assert_and_check_break_state(x, s, v) xu_check_break_state(x, s, v)
#endif

// assert and pass code, not abort it
#ifdef __xu_debug__
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_assertf_pass_return(x, fmt, arg...)                                                                 \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    return;                                                                                            \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_return_val(x, v, fmt, arg...)                                                          \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    return (v);                                                                                        \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_goto(x, b, fmt, arg...)                                                                \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    goto b;                                                                                            \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_break(x, fmt, arg...)                                                                  \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    break;                                                                                             \
                }                                                                                                      \
            }
#        define xu_assertf_pass_continue(x, fmt, arg...)                                                               \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, ##arg);                                                           \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    continue;                                                                                          \
                }                                                                                                      \
            }
#        define xu_assertf_pass_and_check_abort(x, fmt, arg...) xu_assertf_pass_abort(x, fmt, ##arg)
#        define xu_assertf_pass_and_check_return(x, fmt, arg...) xu_assertf_pass_return(x, fmt, ##arg)
#        define xu_assertf_pass_and_check_return_val(x, v, fmt, arg...) xu_assertf_pass_return_val(x, v, fmt, ##arg)
#        define xu_assertf_pass_and_check_goto(x, b, fmt, arg...) xu_assertf_pass_goto(x, b, fmt, ##arg)
#        define xu_assertf_pass_and_check_break(x, fmt, arg...) xu_assertf_pass_break(x, fmt, ##arg)
#        define xu_assertf_pass_and_check_continue(x, fmt, arg...) xu_assertf_pass_continue(x, fmt, ##arg)
#    elif defined(XU_COMPILER_IS_MSVC) && XU_COMPILER_VERSION_BE(13, 0)
#        define xu_assertf_pass_return(x, fmt, ...)                                                                    \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    return;                                                                                            \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_return_val(x, v, fmt, ...)                                                             \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    return (v);                                                                                        \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_goto(x, b, fmt, ...)                                                                   \
            do                                                                                                         \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    goto b;                                                                                            \
                }                                                                                                      \
            } while (0)
#        define xu_assertf_pass_break(x, fmt, ...)                                                                     \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    break;                                                                                             \
                }                                                                                                      \
            }
#        define xu_assertf_pass_continue(x, fmt, ...)                                                                  \
            {                                                                                                          \
                if (!(x))                                                                                              \
                {                                                                                                      \
                    xu_trace_a("expr[%s]: " fmt, #x, __VA_ARGS__);                                                     \
                    xu_assert_backtrace_dump();                                                                        \
                    xu_trace_sync();                                                                                   \
                    continue;                                                                                          \
                }                                                                                                      \
            }
#        define xu_assertf_pass_and_check_abort(x, fmt, ...) xu_assertf_pass_abort(x, fmt, __VA_ARGS__)
#        define xu_assertf_pass_and_check_return(x, fmt, ...) xu_assertf_pass_return(x, fmt, __VA_ARGS__)
#        define xu_assertf_pass_and_check_return_val(x, v, fmt, ...) xu_assertf_pass_return_val(x, v, fmt, __VA_ARGS__)
#        define xu_assertf_pass_and_check_goto(x, b, fmt, ...) xu_assertf_pass_goto(x, b, fmt, __VA_ARGS__)
#        define xu_assertf_pass_and_check_break(x, fmt, ...) xu_assertf_pass_break(x, fmt, __VA_ARGS__)
#        define xu_assertf_pass_and_check_continue(x, fmt, ...) xu_assertf_pass_continue(x, fmt, __VA_ARGS__)
#    else
#        define xu_assertf_pass_return
#        define xu_assertf_pass_return_val
#        define xu_assertf_pass_goto
#        define xu_assertf_pass_break
#        define xu_assertf_pass_continue
#        define xu_assertf_pass_and_check_abort
#        define xu_assertf_pass_and_check_return
#        define xu_assertf_pass_and_check_return_val
#        define xu_assertf_pass_and_check_goto
#        define xu_assertf_pass_and_check_break
#        define xu_assertf_pass_and_check_continue
#    endif
#else
#    if defined(XU_COMPILER_IS_GCC) || defined(XU_COMPILER_IS_TINYC)
#        define xu_assertf_pass_return(x, fmt, arg...)
#        define xu_assertf_pass_return_val(x, v, fmt, arg...)
#        define xu_assertf_pass_goto(x, b, fmt, arg...)
#        define xu_assertf_pass_break(x, fmt, arg...)
#        define xu_assertf_pass_continue(x, fmt, arg...)
#        define xu_assertf_pass_and_check_abort(x, fmt, arg...) xu_check_abort(x)
#        define xu_assertf_pass_and_check_return(x, fmt, arg...) xu_check_return(x)
#        define xu_assertf_pass_and_check_return_val(x, v, fmt, arg...) xu_check_return_val(x, v)
#        define xu_assertf_pass_and_check_goto(x, b, fmt, arg...) xu_check_goto(x, b)
#        define xu_assertf_pass_and_check_break(x, fmt, arg...) xu_check_break(x)
#        define xu_assertf_pass_and_check_continue(x, fmt, arg...) xu_check_continue(x)
#    else
#        define xu_assertf_pass_return
#        define xu_assertf_pass_return_val
#        define xu_assertf_pass_goto
#        define xu_assertf_pass_break
#        define xu_assertf_pass_continue
#        define xu_assertf_pass_and_check_abort
#        define xu_assertf_pass_and_check_return
#        define xu_assertf_pass_and_check_return_val
#        define xu_assertf_pass_and_check_goto
#        define xu_assertf_pass_and_check_break
#        define xu_assertf_pass_and_check_continue
#    endif
#endif

#ifdef __xu_debug__
#    define xu_assert_pass_return(x)                                                                                   \
        do                                                                                                             \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                return;                                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_assert_pass_return_val(x, v)                                                                            \
        do                                                                                                             \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                return (v);                                                                                            \
            }                                                                                                          \
        } while (0)
#    define xu_assert_pass_goto(x, b)                                                                                  \
        do                                                                                                             \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                goto b;                                                                                                \
            }                                                                                                          \
        } while (0)
#    define xu_assert_pass_break(x)                                                                                    \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                break;                                                                                                 \
            }                                                                                                          \
        }
#    define xu_assert_pass_continue(x)                                                                                 \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                continue;                                                                                              \
            }                                                                                                          \
        }
#    define xu_assert_pass_break_state(x, s, v)                                                                        \
        {                                                                                                              \
            if (!(x))                                                                                                  \
            {                                                                                                          \
                (s) = (v);                                                                                             \
                xu_trace_a("expr[%s]", #x);                                                                            \
                xu_assert_backtrace_dump();                                                                            \
                xu_trace_sync();                                                                                       \
                break;                                                                                                 \
            }                                                                                                          \
        }

#    define xu_assert_pass_and_check_abort(x) xu_assert_pass_abort(x)
#    define xu_assert_pass_and_check_return(x) xu_assert_pass_return(x)
#    define xu_assert_pass_and_check_return_val(x, v) xu_assert_pass_return_val(x, v)
#    define xu_assert_pass_and_check_goto(x, b) xu_assert_pass_goto(x, b)
#    define xu_assert_pass_and_check_break(x) xu_assert_pass_break(x)
#    define xu_assert_pass_and_check_continue(x) xu_assert_pass_continue(x)
#    define xu_assert_pass_and_check_break_state(x, s, v) xu_assert_pass_break_state(x, s, v)
#else
#    define xu_assert_pass_return(x)
#    define xu_assert_pass_return_val(x, v)
#    define xu_assert_pass_goto(x, b)
#    define xu_assert_pass_break(x)
#    define xu_assert_pass_continue(x)
#    define xu_assert_pass_break_state(x, s, v)

#    define xu_assert_pass_and_check_abort(x) xu_check_abort(x)
#    define xu_assert_pass_and_check_return(x) xu_check_return(x)
#    define xu_assert_pass_and_check_return_val(x, v) xu_check_return_val(x, v)
#    define xu_assert_pass_and_check_goto(x, b) xu_check_goto(x, b)
#    define xu_assert_pass_and_check_break(x) xu_check_break(x)
#    define xu_assert_pass_and_check_continue(x) xu_check_continue(x)
#    define xu_assert_pass_and_check_break_state(x, s, v) xu_check_break_state(x, s, v)
#endif

// assert: noimpl
#define xu_assert_noimpl() xu_assertf(0, "noimpl")

/*! the static assert
 *
 * @code
 *
 * xu_assert_static(sizeof(xu_uint32_t) == 4);
 *
 * @endcode
 */
#if __xu_has_feature__(c_static_assert)
#    define xu_assert_static(x) _Static_assert(x, "")
#    define xu_assert_static_msg(x, m) _Static_assert(x, m)
#elif defined(__cpp_static_assert)
#    define xu_assert_static(x) static_assert(x, "")
#    define xu_assert_static_msg(x, m) static_assert(x, m)
#else
#    define xu_assert_static(x)                                                                                        \
        do                                                                                                             \
        {                                                                                                              \
            typedef int                          __xu_static_assert__[(x) ? 1 : -1];                                   \
            __xu_volatile__ __xu_static_assert__ __a = {0};                                                            \
            xu_used_ptr((xu_cpointer_t)(xu_size_t)__a);                                                                \
        } while (0)
#    define xu_assert_static_msg(x, m) xu_assert_static(x)
#endif

/* *******************************************************
 * declaration
 */

/*! dump backtrace
 *
 * @param prefix    the prefix tag
 * @param frames    the frame list
 * @param nframe    the frame count
 */
xu_void_t xu_backtrace_dump(xu_char_t const* prefix, xu_pointer_t* frames, xu_size_t nframe);

/*! this variable have been used
 *
 * @param variable  the variable
 */
xu_void_t xu_used_ptr(xu_cpointer_t variable);

/* *******************************************************
 * inlines
 */

#ifdef __xu_debug__
/* the assert implementation
 *
 * @code
 * xu_size_t v = (xu_assert(x), value);
 * xu_assert(v);
 * @endcode
 *
 * @param x                     the boolean value of the expression
 * @param expr                  the expression string
 *
 * @return                      the boolean value of the expression
 */
static __xu_inline__ xu_bool_t xu_assert_impl(xu_bool_t x, xu_char_t const* expr __xu_debug_decl__)
{
    // failed?
    if (!x)
    {
        // trace
        xu_trace_a("expr[%s] at %s(): %s:%d", expr __xu_debug_args__);

        // dump backtrace
        xu_assert_backtrace_dump();

        // sync trace
        xu_trace_sync();

        // abort it
        xu_abort();
    }

    // ok?
    return x;
}
#else
/* the assert empty implementation
 *
 * @return                      only xu_false
 */
static __xu_inline__ xu_bool_t xu_assert_empty_impl(xu_noarg_t) { return xu_false; }
#endif
