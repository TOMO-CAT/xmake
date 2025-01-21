#include "xutil/libc/string/string.h"

#ifndef XU_CONFIG_LIBC_HAVE_STRLCPY
#    if defined(XU_ARCH_x86)
#        include "xutil/libc/string/impl/x86/strlcpy.c"
#    endif
#else
#    include <string.h>
#endif

/* *******************************************************
 *  macros
 * *******************************************************
 */

/* suppress warning as error for clang compiler temporarily:
 *
 * implicit declaration of function 'strlcpy' is invalid in C99 [-Werror,-Wimplicit-function-declaration]
 *
 * TODO: need improve xmake to check this interface more correctly.
 */
#if defined(XU_CONFIG_LIBC_HAVE_STRLCPY) && defined(XU_COMPILER_IS_CLANG)
#    undef XU_CONFIG_LIBC_HAVE_STRLCPY
#endif

/* *******************************************************
 *  implementation
 * *******************************************************
 */

#if defined(XU_CONFIG_LIBC_HAVE_STRLCPY)
static xu_size_t xu_strlcpy_impl(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);

    // copy it
    return strlcpy(s1, s2, n);
}
#elif !defined(XU_LIBC_STRING_IMPL_STRLCPY)
/* copy s2 to s1 of size n
 *
 * - at most n - 1 characters will be copied.
 * - always null terminates (unless n == 0).
 *
 * returns strlen(s2); if retval >= n, truncation occurred.
 */
static xu_size_t xu_strlcpy_impl(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);

    // init
    xu_char_t*       d = s1;
    xu_char_t const* s = s2;
    xu_size_t        m = n;

    // copy as many bytes as will fit
    if (m != 0 && --m != 0)
    {
        do
        {
            if ((*d++ = *s++) == 0) break;

        } while (--m != 0);
    }

    // not enough room in dst, add null and traverse rest of src
    if (m == 0)
    {
        if (n != 0) *d = '\0';
        while (*s++)
            ;
    }

    // count does not include null
    return (s - s2 - 1);
}
#endif

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

xu_size_t xu_strlcpy(xu_char_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
#ifdef __xu_debug__
    {
        // overflow dst?
        xu_size_t n2 = xu_strlen(s2);

        // strlcpy overflow?
        xu_size_t n1 = xu_pool_data_size(s1);
        if (n1 && xu_min(n2 + 1, n) > n1)
        {
            xu_trace_i("[strlcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, xu_min(n2 + 1, n), s1, n1);
            xu_backtrace_dump("[strlcpy]: [overflow]: ", xu_null, 10);
            xu_pool_data_dump(s2, xu_true, "\t[malloc]: [from]: ");
            xu_abort();
        }
    }
#endif

    // done
    return xu_strlcpy_impl(s1, s2, n);
}
