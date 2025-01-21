#pragma once

#include "xutil/memory/buffer.h"
#include "xutil/memory/memory.h"
#include "xutil/prefix.h"
#include "xutil/string/static_string.h"

/* *******************************************************
 *  types
 * *******************************************************
 */

// the string type
typedef xu_buffer_t xu_string_t;

// the string ref type
typedef xu_buffer_ref_t xu_string_ref_t;

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
/*! init string
 *
 * @param string        the string
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_string_init(xu_string_ref_t string);

/*! exit string
 *
 * @param string        the string
 */
xu_void_t xu_string_exit(xu_string_ref_t string);

/*! the c-string pointer
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstr(xu_string_ref_t string);

/*! the string size
 *
 * @param string        the string
 *
 * @return              the string size
 */
xu_size_t xu_string_size(xu_string_ref_t string);

/*! clear the string
 *
 * @param string        the string
 */
xu_void_t xu_string_clear(xu_string_ref_t string);

/*! strip the string
 *
 * @param string        the string
 * @param n             the striped size
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_strip(xu_string_ref_t string, xu_size_t n);

/*! trim the left spaces for string
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_ltrim(xu_string_ref_t string);

/*! trim the right spaces for string
 *
 * @param string        the string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_rtrim(xu_string_ref_t string);

/*! get the charactor at the given position
 *
 * @param string        the string
 * @param p             the position
 *
 * @return              the c-string
 */
xu_char_t xu_string_charat(xu_string_ref_t string, xu_size_t p);

/*! find charactor position
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strchr(xu_string_ref_t string, xu_size_t p, xu_char_t c);

/*! find charactor position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strichr(xu_string_ref_t string, xu_size_t p, xu_char_t c);

/*! reverse to find charactor position
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strrchr(xu_string_ref_t string, xu_size_t p, xu_char_t c);

/*! reverse to find charactor position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param c             the found charactor
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strirchr(xu_string_ref_t string, xu_size_t p, xu_char_t c);

/*! find string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s);

/*! find string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_stristr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s);

/*! find c-string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_cstrstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! find c-string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_cstristr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! reverse to find string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strrstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s);

/*! reverse to find string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_strirstr(xu_string_ref_t string, xu_size_t p, xu_string_ref_t s);

/*! reverse to find c-string position
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_cstrrstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! reverse to find c-string position and ignore case
 *
 * @param string        the string
 * @param p             the start position
 * @param s             the found c-string
 *
 * @return              the real position, no find: -1
 */
xu_long_t xu_string_cstrirstr(xu_string_ref_t string, xu_size_t p, xu_char_t const* s);

/*! copy string
 *
 * @param string        the string
 * @param s             the copied string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_strcpy(xu_string_ref_t string, xu_string_ref_t s);

/*! copy c-string
 *
 * @param string        the string
 * @param s             the copied c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrcpy(xu_string_ref_t string, xu_char_t const* s);

/*! copy c-string with the given size
 *
 * @param string        the string
 * @param s             the copied c-string
 * @param n             the copied c-string size
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrncpy(xu_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! copy format c-string
 *
 * @param string        the string
 * @param fmt           the copied format c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrfcpy(xu_string_ref_t string, xu_char_t const* fmt, ...);

/*! append charactor
 *
 * @param string        the string
 * @param c             the appended charactor
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_chrcat(xu_string_ref_t string, xu_char_t c);

/*! append charactor with the given size
 *
 * @param string        the string
 * @param c             the appended charactor
 * @param n             the appended size
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_chrncat(xu_string_ref_t string, xu_char_t c, xu_size_t n);

/*! append string
 *
 * @param string        the string
 * @param s             the appended string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_strcat(xu_string_ref_t string, xu_string_ref_t s);

/*! append c-string
 *
 * @param string        the string
 * @param s             the appended c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrcat(xu_string_ref_t string, xu_char_t const* s);

/*! append c-string with the given size
 *
 * @param string        the string
 * @param s             the appended c-string
 * @param n             the appended c-string size
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrncat(xu_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! append format c-string
 *
 * @param string        the string
 * @param fmt           the appended format c-string
 *
 * @return              the c-string
 */
xu_char_t const* xu_string_cstrfcat(xu_string_ref_t string, xu_char_t const* fmt, ...);

/*! compare string
 *
 * @param string        the string
 * @param s             the compared string
 *
 * @return              equal: 0
 */
xu_long_t xu_string_strcmp(xu_string_ref_t string, xu_string_ref_t s);

/*! compare string and ignore case
 *
 * @param string        the string
 * @param s             the compared string
 *
 * @return              equal: 0
 */
xu_long_t xu_string_strimp(xu_string_ref_t string, xu_string_ref_t s);

/*! compare c-string
 *
 * @param string        the string
 * @param s             the compared c-string
 *
 * @return              equal: 0
 */
xu_long_t xu_string_cstrcmp(xu_string_ref_t string, xu_char_t const* s);

/*! compare c-string and ignore case
 *
 * @param string        the string
 * @param s             the compared c-string
 *
 * @return              equal: 0
 */
xu_long_t xu_string_cstricmp(xu_string_ref_t string, xu_char_t const* s);

/*! compare c-string with given size
 *
 * @param string        the string
 * @param s             the compared c-string
 * #param n             the compared c-string size
 *
 * @return              equal: 0
 */
xu_long_t xu_string_cstrncmp(xu_string_ref_t string, xu_char_t const* s, xu_size_t n);

/*! compare c-string with given size and ignore case
 *
 * @param string        the string
 * @param s             the compared c-string
 * #param n             the compared c-string size
 *
 * @return              equal: 0
 */
xu_long_t xu_string_cstrnicmp(xu_string_ref_t string, xu_char_t const* s, xu_size_t n);
