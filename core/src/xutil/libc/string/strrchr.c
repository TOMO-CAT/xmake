#include "xutil/libc/string/string.h"
#ifdef XU_CONFIG_LIBC_HAVE_STRRCHR
#   include <string.h>
#endif

/* *******************************************************
 * implementation
 */
#ifdef XU_CONFIG_LIBC_HAVE_STRRCHR
xu_char_t* xu_strrchr(xu_char_t const* s, xu_char_t c)
{
    xu_assert(s);
    return (xu_char_t*)strrchr(s, c);
}
#else
xu_char_t* xu_strrchr(xu_char_t const* s, xu_char_t c)
{
    xu_assert_and_check_return_val(s, xu_null);
    return xu_strnrchr(s, xu_strlen(s), c);
}
#endif
