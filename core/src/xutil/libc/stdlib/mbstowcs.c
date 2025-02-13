#include "xutil/libc/stdlib/stdlib.h"

#ifdef XU_CONFIG_LIBC_HAVE_MBSTOWCS
#    include "xutil/libc/stdlib/setlocale.h"
#    include <stdlib.h>
#endif
#ifdef XU_CONFIG_MODULE_HAVE_CHARSET
#    include "xutil/charset/charset.h"
#endif

/* *******************************************************
 * macros
 */

// select the implementation of mbstowcs
#ifdef XU_CONFIG_FORCE_UTF8
#    if defined(XU_CONFIG_MODULE_HAVE_CHARSET)
#        define XU_MBSTOWCS_IMPL_CHARSET
#    elif defined(XU_CONFIG_LIBC_HAVE_MBSTOWCS)
#        define XU_MBSTOWCS_IMPL_LIBC
#    endif
#else
#    if defined(XU_CONFIG_LIBC_HAVE_MBSTOWCS)
#        define XU_MBSTOWCS_IMPL_LIBC
#    elif defined(XU_CONFIG_MODULE_HAVE_CHARSET)
#        define XU_MBSTOWCS_IMPL_CHARSET
#    endif
#endif

/* *******************************************************
 * implementation
 */
#ifdef XU_MBSTOWCS_IMPL_LIBC
inline static xu_size_t xu_mbstowcs_libc(xu_wchar_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // set local locale
    xu_setlocale();

    // convert it
    n = mbstowcs(s1, s2, n);

    // set default locale
    xu_resetlocale();

    // ok
    return n;
}
#endif

#ifdef XU_MBSTOWCS_IMPL_CHARSET
inline static xu_size_t xu_mbstowcs_charset(xu_wchar_t* s1, xu_char_t const* s2, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(s1 && s2, 0);

    // init
    xu_size_t e = (sizeof(xu_wchar_t) == 4) ? XU_CHARSET_TYPE_UTF32 : XU_CHARSET_TYPE_UTF16;
    xu_long_t r =
        xu_charset_conv_cstr(XU_CHARSET_TYPE_UTF8, e | XU_CHARSET_TYPE_LE, s2, (xu_byte_t*)s1, n * sizeof(xu_wchar_t));
    if (r > 0) r /= sizeof(xu_wchar_t);

    // strip
    if (r >= 0) s1[r] = L'\0';

    // ok?
    return r >= 0 ? r : -1;
}
#endif

/* *******************************************************
 * interfaces
 */
xu_size_t xu_mbstowcs(xu_wchar_t* s1, xu_char_t const* s2, xu_size_t n)
{
#if defined(XU_MBSTOWCS_IMPL_CHARSET)
    return xu_mbstowcs_charset(s1, s2, n);
#elif defined(XU_MBSTOWCS_IMPL_LIBC)
    return xu_mbstowcs_libc(s1, s2, n);
#else
    xu_trace_noimpl();
    return -1;
#endif
}
