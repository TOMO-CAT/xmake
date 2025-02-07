#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the stdfile ref type
typedef __xu_typeref__(stdfile);

// the stdfile type
typedef enum __xu_stdfile_type_e
{
    XU_STDFILE_TYPE_NONE    = 0
,   XU_STDFILE_TYPE_STDIN   = 1
,   XU_STDFILE_TYPE_STDOUT  = 2
,   XU_STDFILE_TYPE_STDERR  = 3

}xu_stdfile_type_e;

/* *******************************************************
 * interfaces
 */

/*! get stdin file singleton
 *
 * @return              the stdin file singleton
 */
xu_stdfile_ref_t        xu_stdfile_input(xu_noarg_t);

/*! get stdout file singleton
 *
 * @return              the stdout file singleton
 */
xu_stdfile_ref_t        xu_stdfile_output(xu_noarg_t);

/*! get stderr file singleton
 *
 * @return              the stdin file singleton
 */
xu_stdfile_ref_t        xu_stdfile_error(xu_noarg_t);

/*! init the standard input/ouput device file
 *
 * @param type          the file type, e.g. stdin, stdout, stderr
 *
 * @return              the stdfile
 */
xu_stdfile_ref_t        xu_stdfile_init(xu_size_t type);

/*! exit the stdfile
 *
 * @param stdfile       the stdfile
 */
xu_void_t               xu_stdfile_exit(xu_stdfile_ref_t stdfile);

/*! get the stdfile type
 *
 * @param stdfile       the stdfile
 *
 * @return              the file type
 */
xu_size_t               xu_stdfile_type(xu_stdfile_ref_t stdfile);

/*! forces a write of all buffered data for the given output
 *
 * @note does not support to flush stdin
 *
 * @param file          the file
 */
xu_bool_t               xu_stdfile_flush(xu_stdfile_ref_t stdfile);

/*! read the stdfile data (block mode)
 *
 * @param file          the stdfile
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_read(xu_stdfile_ref_t file, xu_byte_t* data, xu_size_t size);

/*! write the stdfile data (block mode)
 *
 * @param file          the stdfile
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_writ(xu_stdfile_ref_t file, xu_byte_t const* data, xu_size_t size);

/*! has readable data for stdfile
 *
 * @param file          the stdfile
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_readable(xu_stdfile_ref_t file);

/*! peek an character from stdfile
 *
 * @param file          the stdfile
 * @param ch            the character pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_peek(xu_stdfile_ref_t file, xu_char_t* pch);

/*! read an character from stdfile
 *
 * @param file          the stdfile
 * @param ch            the character pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_getc(xu_stdfile_ref_t file, xu_char_t* pch);

/*! write an character to stdfile
 *
 * @param file          the stdfile
 * @param ch            the character
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_putc(xu_stdfile_ref_t file, xu_char_t ch);

/*! reads characters from stdfile and stores them as a C string into str until (num-1) characters have been read
 * or either a newline or the end-of-file is reached, whichever happens first.
 *
 * a newline character makes fgets stop reading, but it is considered a valid character by the function and
 * included in the string copied to str.
 *
 * a terminating null character is automatically appended after the characters copied to str.
 *
 * @param file          the stdfile
 * @param str           pointer to an array of chars where the string read is copied.
 * @param num           maximum number of characters to be copied into str (including the terminating null-character).
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_gets(xu_stdfile_ref_t file, xu_char_t* str, xu_size_t num);

/*! writes the C string pointed by str to the stdfile.
 *
 * the function begins copying from the address specified (str) until it reaches the terminating null character ('\0').
 * this terminating null-character is not copied to the stdfile.
 *
 * @param file          the stdfile
 * @param s             C string with the content to be written to stdfile.
 *
 * @return              xu_true or xu_false
 */
xu_bool_t               xu_stdfile_puts(xu_stdfile_ref_t file, xu_char_t const* str);
