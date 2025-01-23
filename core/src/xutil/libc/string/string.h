#pragma once

#include "xutil/libc/misc/ctype.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/type.h"
#include "xutil/prefix.h"
#include "xutil/utils/bits.h"

/* *******************************************************
 *  macros
 */
// memset_ptr
#if XU_CPU_BIT64
#    define xu_memset_ptr(s, p, n) xu_memset_u64(s, (xu_uint64_t)(p), n)
#else
#    define xu_memset_ptr(s, p, n) xu_memset_u32(s, (xu_uint32_t)(p), n)
#endif

/* *******************************************************
 *  interfaces
 */

// memset
xu_pointer_t xu_memset(xu_pointer_t s, xu_byte_t c, xu_size_t n);
xu_pointer_t xu_memset_(xu_pointer_t s, xu_byte_t c, xu_size_t n);
xu_pointer_t xu_memset_u16(xu_pointer_t s, xu_uint16_t c, xu_size_t n);
xu_pointer_t xu_memset_u24(xu_pointer_t s, xu_uint32_t c, xu_size_t n);
xu_pointer_t xu_memset_u32(xu_pointer_t s, xu_uint32_t c, xu_size_t n);
xu_pointer_t xu_memset_u64(xu_pointer_t s, xu_uint64_t c, xu_size_t n);

// memdup
xu_pointer_t xu_memdup(xu_cpointer_t s, xu_size_t n);
xu_pointer_t xu_memdup_(xu_cpointer_t s, xu_size_t n);

// memcpy
xu_pointer_t xu_memcpy(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n);
xu_pointer_t xu_memcpy_(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n);

// memmov
xu_pointer_t xu_memmov(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n);
xu_pointer_t xu_memmov_(xu_pointer_t s1, xu_cpointer_t s2, xu_size_t n);

// memcmp
xu_long_t xu_memcmp(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n);
xu_long_t xu_memcmp_(xu_cpointer_t s1, xu_cpointer_t s2, xu_size_t n);

// memmem
xu_pointer_t xu_memmem(xu_cpointer_t s1, xu_size_t n1, xu_cpointer_t s2, xu_size_t n2);
xu_pointer_t xu_memmem_(xu_cpointer_t s1, xu_size_t n1, xu_cpointer_t s2, xu_size_t n2);

// strlen
xu_size_t xu_strlen(xu_char_t const* s);
xu_size_t xu_strnlen(xu_char_t const* s, xu_size_t n);

// strdup
xu_char_t* xu_strdup(xu_char_t const* s);
xu_char_t* xu_strndup(xu_char_t const* s, xu_size_t n);

// strcat
xu_char_t* xu_strcat(xu_char_t* s1, xu_char_t const* s2);
xu_char_t* xu_strncat(xu_char_t* s1, xu_char_t const* s2, xu_size_t n);

// strcpy
xu_char_t* xu_strcpy(xu_char_t* s1, xu_char_t const* s2);
xu_char_t* xu_strncpy(xu_char_t* s1, xu_char_t const* s2, xu_size_t n);
xu_size_t  xu_strlcpy(xu_char_t* s1, xu_char_t const* s2, xu_size_t n);

// strcmp
xu_long_t xu_strcmp(xu_char_t const* s1, xu_char_t const* s2);
xu_long_t xu_strncmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n);

xu_long_t xu_stricmp(xu_char_t const* s1, xu_char_t const* s2);
xu_long_t xu_strnicmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t n);

// strchr
xu_char_t* xu_strchr(xu_char_t const* s, xu_char_t c);
xu_char_t* xu_strichr(xu_char_t const* s, xu_char_t c);

xu_char_t* xu_strnchr(xu_char_t const* s, xu_size_t n, xu_char_t c);
xu_char_t* xu_strnichr(xu_char_t const* s, xu_size_t n, xu_char_t c);

// strrchr
xu_char_t* xu_strrchr(xu_char_t const* s, xu_char_t c);
xu_char_t* xu_strirchr(xu_char_t const* s, xu_char_t c);

xu_char_t* xu_strnrchr(xu_char_t const* s, xu_size_t n, xu_char_t c);
xu_char_t* xu_strnirchr(xu_char_t const* s, xu_size_t n, xu_char_t c);

// strstr
xu_char_t* xu_strstr(xu_char_t const* s1, xu_char_t const* s2);
xu_char_t* xu_stristr(xu_char_t const* s1, xu_char_t const* s2);

xu_char_t* xu_strnstr(xu_char_t const* s1, xu_size_t n1, xu_char_t const* s2);
xu_char_t* xu_strnistr(xu_char_t const* s1, xu_size_t n1, xu_char_t const* s2);

// strrstr
xu_char_t* xu_strrstr(xu_char_t const* s1, xu_char_t const* s2);
xu_char_t* xu_strirstr(xu_char_t const* s1, xu_char_t const* s2);

xu_char_t* xu_strnrstr(xu_char_t const* s1, xu_size_t n1, xu_char_t const* s2);
xu_char_t* xu_strnirstr(xu_char_t const* s1, xu_size_t n1, xu_char_t const* s2);

// wcslen
xu_size_t xu_wcslen(xu_wchar_t const* s);
xu_size_t xu_wcsnlen(xu_wchar_t const* s, xu_size_t n);

// wcsdup
xu_wchar_t* xu_wcsdup(xu_wchar_t const* s);
xu_wchar_t* xu_wcsndup(xu_wchar_t const* s, xu_size_t n);

// wcscat
xu_wchar_t* xu_wcscat(xu_wchar_t* s1, xu_wchar_t const* s2);
xu_wchar_t* xu_wcsncat(xu_wchar_t* s1, xu_wchar_t const* s2, xu_size_t n);

// wcscpy
xu_wchar_t* xu_wcscpy(xu_wchar_t* s1, xu_wchar_t const* s2);
xu_wchar_t* xu_wcsncpy(xu_wchar_t* s1, xu_wchar_t const* s2, xu_size_t n);
xu_size_t   xu_wcslcpy(xu_wchar_t* s1, xu_wchar_t const* s2, xu_size_t n);

// wcscmp
xu_long_t xu_wcscmp(xu_wchar_t const* s1, xu_wchar_t const* s2);
xu_long_t xu_wcsncmp(xu_wchar_t const* s1, xu_wchar_t const* s2, xu_size_t n);

xu_long_t xu_wcsicmp(xu_wchar_t const* s1, xu_wchar_t const* s2);
xu_long_t xu_wcsnicmp(xu_wchar_t const* s1, xu_wchar_t const* s2, xu_size_t n);

// wcschr
xu_wchar_t* xu_wcschr(xu_wchar_t const* s, xu_wchar_t c);
xu_wchar_t* xu_wcsichr(xu_wchar_t const* s, xu_wchar_t c);

// wcsrchr
xu_wchar_t* xu_wcsrchr(xu_wchar_t const* s, xu_wchar_t c);
xu_wchar_t* xu_wcsirchr(xu_wchar_t const* s, xu_wchar_t c);

xu_wchar_t* xu_wcsnrchr(xu_wchar_t const* s, xu_size_t n, xu_wchar_t c);
xu_wchar_t* xu_wcsnirchr(xu_wchar_t const* s, xu_size_t n, xu_wchar_t c);

// wcsstr
xu_wchar_t* xu_wcsstr(xu_wchar_t const* s1, xu_wchar_t const* s2);
xu_wchar_t* xu_wcsistr(xu_wchar_t const* s1, xu_wchar_t const* s2);

// wcsrstr
xu_wchar_t* xu_wcsrstr(xu_wchar_t const* s1, xu_wchar_t const* s2);
xu_wchar_t* xu_wcsirstr(xu_wchar_t const* s1, xu_wchar_t const* s2);

xu_wchar_t* xu_wcsnrstr(xu_wchar_t const* s1, xu_size_t n, xu_wchar_t const* s2);
xu_wchar_t* xu_wcsnirstr(xu_wchar_t const* s1, xu_size_t n, xu_wchar_t const* s2);
