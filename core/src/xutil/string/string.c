#include "xutil/string/string.h"

/* *******************************************************
 *  macros
 */

// the format string data size
#ifdef __xu_small__
#    define XU_SCOPED_STRING_FMTD_SIZE (512)
#else
#    define XU_SCOPED_STRING_FMTD_SIZE (1024)
#endif

/* *******************************************************
 *  implementation
 */

xu_bool_t xu_string_init(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return_val(string, xu_false);

    // init
    xu_bool_t ok = xu_buffer_init(string);

    // clear it
    xu_string_clear(string);

    // ok?
    return ok;
}
xu_void_t xu_string_exit(xu_string_ref_t string)
{
    if (string) xu_buffer_exit(string);
}
xu_char_t const* xu_string_cstr(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // the cstr
    return xu_string_size(string) ? (xu_char_t const*)xu_buffer_data((xu_buffer_ref_t)string) : xu_null;
}
xu_size_t xu_string_size(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return_val(string, 0);

    // the size
    xu_size_t n = xu_buffer_size(string);
    return n > 0 ? n - 1 : 0;
}
xu_void_t xu_string_clear(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return(string);

    // clear buffer
    xu_buffer_clear(string);

    // clear string
    xu_char_t* p = (xu_char_t*)xu_buffer_data(string);
    if (p) p[0] = '\0';
}
xu_char_t const* xu_string_strip(xu_string_ref_t string, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // out?
    xu_check_return_val(n < xu_string_size(string), xu_string_cstr(string));

    // strip
    xu_char_t* p = (xu_char_t*)xu_buffer_resize(string, n + 1);
    if (p) p[n] = '\0';

    // ok?
    return p;
}
xu_char_t const* xu_string_ltrim(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // init
    xu_char_t* s = (xu_char_t*)xu_string_cstr(string);
    xu_size_t  n = xu_string_size(string);
    xu_check_return_val(s && n, xu_null);

    // done
    xu_char_t* p = s;
    xu_char_t* e = s + n;
    while (p < e && xu_isspace(*p))
        p++;

    // strip it
    if (p < e)
    {
        // move it if exists spaces
        if (p > s) xu_buffer_memmov(string, p - s);
    }
    // clear it
    else
        xu_string_clear(string);

    // ok?
    return xu_string_cstr(string);
}
xu_char_t const* xu_string_rtrim(xu_string_ref_t string)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // init
    xu_char_t* s = (xu_char_t*)xu_string_cstr(string);
    xu_size_t  n = xu_string_size(string);
    xu_check_return_val(s && n, xu_null);

    // done
    xu_char_t* e = s + n - 1;
    while (e >= s && xu_isspace(*e))
        e--;

    // strip it
    if (e >= s) xu_string_strip(string, e - s + 1);
    // clear it
    else
        xu_string_clear(string);

    // ok?
    return xu_string_cstr(string);
}
xu_char_t xu_string_charat(xu_string_ref_t string, xu_size_t p)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, '\0');

    // get it
    return s[p];
}
xu_long_t xu_string_strchr(xu_string_ref_t string, xu_size_t p, xu_char_t c)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strchr(s + p, c);
    return (q ? q - s : -1);
}
xu_long_t xu_string_strichr(xu_string_ref_t string, xu_size_t p, xu_char_t c)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strichr(s + p, c);
    return (q ? q - s : -1);
}
xu_long_t xu_string_strrchr(xu_string_ref_t string, xu_size_t p, xu_char_t c)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strnrchr(s + p, n, c);
    return (q ? q - s : -1);
}
xu_long_t xu_string_strirchr(xu_string_ref_t string, xu_size_t p, xu_char_t c)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strnirchr(s + p, n, c);
    return (q ? q - s : -1);
}
xu_long_t xu_string_strstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s)
{
    return xu_string_cstrstr(string, p, xu_string_cstr(s));
}
xu_long_t xu_string_stristr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s)
{
    return xu_string_cstristr(string, p, xu_string_cstr(s));
}
xu_long_t xu_string_cstrstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s2)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strstr(s + p, s2);
    return (q ? q - s : -1);
}
xu_long_t xu_string_cstristr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s2)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_stristr(s + p, s2);
    return (q ? q - s : -1);
}
xu_long_t xu_string_strrstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s)
{
    return xu_string_cstrrstr(string, p, xu_string_cstr(s));
}
xu_long_t xu_string_strirstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s)
{
    return xu_string_cstrirstr(string, p, xu_string_cstr(s));
}
xu_long_t xu_string_cstrrstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s2)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strnrstr(s + p, n, s2);
    return (q ? q - s : -1);
}
xu_long_t xu_string_cstrirstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s2)
{
    // check
    xu_char_t const* s = xu_string_cstr(string);
    xu_size_t        n = xu_string_size(string);
    xu_assert_and_check_return_val(s && p < n, -1);

    // done
    xu_char_t* q = xu_strnirstr(s + p, n, s2);
    return (q ? q - s : -1);
}
xu_char_t const* xu_string_strcpy(xu_string_ref_t string, xu_string_ref_t s)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // done
    xu_size_t n = xu_string_size(s);
    if (n)
        return xu_string_cstrncpy(string, xu_string_cstr(s), n);
    else
    {
        xu_string_clear(string);
        return xu_null;
    }
}
xu_char_t const* xu_string_cstrcpy(xu_string_ref_t string, xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);
    return xu_string_cstrncpy(string, s, xu_strlen(s));
}
xu_char_t const* xu_string_cstrncpy(xu_string_ref_t string, xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string && s, xu_null);
    xu_check_return_val(n, xu_string_cstr(string));

    xu_char_t* p = (xu_char_t*)xu_buffer_memncpy(string, (xu_byte_t const*)s, n + 1);
    if (p) p[xu_string_size(string)] = '\0';
    return p;
}
xu_char_t const* xu_string_cstrfcpy(xu_string_ref_t string, xu_char_t const* fmt, ...)
{
    // check
    xu_assert_and_check_return_val(string && fmt, xu_null);

    // format data
    xu_char_t p[XU_SCOPED_STRING_FMTD_SIZE] = {0};
    xu_size_t n                             = 0;
    xu_vsnprintf_format(p, XU_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    xu_assert_and_check_return_val(n, xu_null);

    // done
    return xu_string_cstrncpy(string, p, n);
}
xu_char_t const* xu_string_chrcat(xu_string_ref_t string, xu_char_t c)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // done
    xu_char_t* p = (xu_char_t*)xu_buffer_memnsetp(string, xu_string_size(string), c, 2);
    if (p) p[xu_string_size(string)] = '\0';
    return p;
}
xu_char_t const* xu_string_chrncat(xu_string_ref_t string, xu_char_t c, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string, xu_null);

    // done
    xu_char_t* p = (xu_char_t*)xu_buffer_memnsetp(string, xu_string_size(string), c, n + 1);
    if (p) p[xu_string_size(string)] = '\0';
    return p;
}
xu_char_t const* xu_string_strcat(xu_string_ref_t string, xu_string_ref_t s)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);

    // done
    return xu_string_cstrncat(string, xu_string_cstr(s), xu_string_size(s));
}
xu_char_t const* xu_string_cstrcat(xu_string_ref_t string, xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(s, xu_null);
    return xu_string_cstrncat(string, s, xu_strlen(s));
}
xu_char_t const* xu_string_cstrncat(xu_string_ref_t string, xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string && s, xu_null);
    xu_check_return_val(n, xu_string_cstr(string));

    // done
    xu_char_t* p = (xu_char_t*)xu_buffer_memncpyp(string, xu_string_size(string), (xu_byte_t const*)s, n + 1);
    if (p) p[xu_string_size(string)] = '\0';
    return p;
}
xu_char_t const* xu_string_cstrfcat(xu_string_ref_t string, xu_char_t const* fmt, ...)
{
    // check
    xu_assert_and_check_return_val(string && fmt, xu_null);

    // format data
    xu_char_t p[XU_SCOPED_STRING_FMTD_SIZE] = {0};
    xu_long_t n                             = 0;
    xu_vsnprintf_format(p, XU_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    xu_assert_and_check_return_val(n, xu_null);

    // done
    return xu_string_cstrncat(string, p, n);
}
xu_long_t xu_string_strcmp(xu_string_ref_t string, xu_string_ref_t s)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_string_cstrncmp(string, xu_string_cstr(s), xu_string_size(s) + 1);
}
xu_long_t xu_string_strimp(xu_string_ref_t string, xu_string_ref_t s)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_string_cstrnicmp(string, xu_string_cstr(s), xu_string_size(s) + 1);
}
xu_long_t xu_string_cstrcmp(xu_string_ref_t string, xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_string_cstrncmp(string, s, xu_strlen(s) + 1);
}
xu_long_t xu_string_cstricmp(xu_string_ref_t string, xu_char_t const* s)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_string_cstrnicmp(string, s, xu_strlen(s) + 1);
}
xu_long_t xu_string_cstrncmp(xu_string_ref_t string, xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_strncmp(xu_string_cstr(string), s, n);
}
xu_long_t xu_string_cstrnicmp(xu_string_ref_t string, xu_char_t const* s, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(string && s, 0);
    return xu_strnicmp(xu_string_cstr(string), s, n);
}
