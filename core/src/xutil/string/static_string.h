#pragma once

#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/memory/memory.h"
#include "xutil/prefix.h"

/* *******************************************************
 *  types
 */

// the static string type
typedef xu_static_buffer_t xu_static_string_t;

// the static string ref type
typedef xu_static_buffer_ref_t xu_static_string_ref_t;

/* *******************************************************
 *  interfaces
 */
/*! init string
 *
 * @param string        the string
 * @param data          the data
 * @param maxn          the maxn
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_static_string_init(xu_static_string_ref_t string, xu_char_t* data, xu_size_t maxn);

/*! exit string
 *
 * @param string        the string
 */
xu_void_t xu_static_string_exit(xu_static_string_ref_t string);

/*! the c-string pointer
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstr(xu_static_string_ref_t string);

/*! the string size
 *
 * @param string        the string
 *
 * @return              the string size
 */
xu_size_t xu_static_string_size(xu_static_string_ref_t string);

/*! clear the string
 *
 * @param string        the string
 */
xu_void_t xu_static_string_clear(xu_static_string_ref_t string);

/*! strip the string
 *
 * @param string        the string
 * @param n             the striped size
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_strip(xu_static_string_ref_t string, xu_size_t n);

/*! trim the left spaces for string
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_ltrim(xu_static_string_ref_t string);

/*! trim the right spaces for string
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_rtrim(xu_static_string_ref_t string);

/*! get the charactor at the given position
 *
 * @param string        the string
 * @param p             the position
 *
 * @return              the c-string
 */
xu_char_t xu_static_string_charat(xu_static_string_ref_t string, xu_size_t p);

/*! find charactor position
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strchr(xu_static_string_ref_t string, xu_size_t p, xu_char_t c);

/*! find charactor position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strichr(xu_static_string_ref_t string, xu_size_t p, xu_char_t c);

/*! reverse to find charactor position
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strrchr(xu_static_string_ref_t string, xu_size_t p, xu_char_t c);

/*! reverse to find charactor position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strirchr(xu_static_string_ref_t string, xu_size_t p, xu_char_t c);

/*! find string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strstr(xu_static_string_ref_t string, xu_size_t p, xu_static_string_ref_t s);

/*! find string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_stristr(xu_static_string_ref_t string, xu_size_t p, xu_static_string_ref_t s);

/*! find c-string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_cstrstr(xu_static_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! find c-string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_cstristr(xu_static_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! reverse to find string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strrstr(xu_static_string_ref_t string, xu_size_t p, xu_static_string_ref_t s);

/*! reverse to find string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_strirstr(xu_static_string_ref_t string, xu_size_t p, xu_static_string_ref_t s);

/*! reverse to find c-string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_cstrrstr(xu_static_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! reverse to find c-string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_static_string_cstrirstr(xu_static_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! copy string
 *
 * @param string        the string
 * @param s             the copied string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_strcpy(xu_static_string_ref_t string, xu_static_string_ref_t s);

/*! copy c-string
 *
 * @param string        the string
 * @param s             the copied c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrcpy(xu_static_string_ref_t string, xu_char_t const* s);

/*! copy c-string with the given size
 *
 * @param string        the string
 * @param s             the copied c-string
 * @param n             the copied c-string size
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrncpy(xu_static_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! copy format c-string
 *
 * @param string        the string
 * @param fmt           the copied format c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrfcpy(xu_static_string_ref_t string, xu_char_t const* fmt, ...);

/*! append charactor
 *
 * @param string        the string
 * @param c             the appended charactor
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_chrcat(xu_static_string_ref_t string, xu_char_t c);

/*! append charactor with the given size
 *
 * @param string        the string
 * @param c             the appended charactor
 * @param n             the appended size
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_chrncat(xu_static_string_ref_t string, xu_char_t c, xu_size_t n);

/*! append string
 *
 * @param string        the string
 * @param s             the appended string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_strcat(xu_static_string_ref_t string, xu_static_string_ref_t s);

/*! append c-string
 *
 * @param string        the string
 * @param s             the appended c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrcat(xu_static_string_ref_t string, xu_char_t const* s);

/*! append c-string with the given size
 *
 * @param string        the string
 * @param s             the appended c-string
 * @param n             the appended c-string size
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrncat(xu_static_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! append format c-string
 *
 * @param string        the string
 * @param fmt           the appended format c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_static_string_cstrfcat(xu_static_string_ref_t string, xu_char_t const* fmt, ...);

/*! compare string
 *
 * @param string        the string
 * @param s             the compared string
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_strcmp(xu_static_string_ref_t string, xu_static_string_ref_t s);

/*! compare string and ignore case
 *
 * @param string        the string
 * @param s             the compared string
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_strimp(xu_static_string_ref_t string, xu_static_string_ref_t s);

/*! compare c-string
 *
 * @param string        the string
 * @param s             the compared c-string
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_cstrcmp(xu_static_string_ref_t string, xu_char_t const* s);

/*! compare c-string and ignore case
 *
 * @param string        the string
 * @param s             the compared c-string
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_cstricmp(xu_static_string_ref_t string, xu_char_t const* s);

/*! compare c-string with given size
 *
 * @param string        the string
 * @param s             the compared c-string
 * #param n             the compared c-string size
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_cstrncmp(xu_static_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! compare c-string with given size and ignore case
 *
 * @param string        the string
 * @param s             the compared c-string
 * #param n             the compared c-string size
 *
 * @return              equal: 0
 */
xu_long_t xu_static_string_cstrnicmp(xu_static_string_ref_t string, xu_char_t const* s, xu_size_t n);
