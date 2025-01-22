#pragma once

#include "xutil/prefix.h"

/* *******************************************************
 *  includes
 */

#include "xutil/libc/misc/ctype.h"
#include "xutil/libc/stdio/printf_object.h"
#include "xutil/libm/math.h"
#include "xutil/macros/type.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 *  macros
 */

// vsnprintf format
#define xu_vsnprintf_format(s, n, format, r)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_long_t    __xu_ret = 0;                                                                                     \
        xu_va_list_t __xu_varg_list;                                                                                   \
        xu_va_start(__xu_varg_list, format);                                                                           \
        __xu_ret = xu_vsnprintf(s, (n), format, __xu_varg_list);                                                       \
        xu_va_end(__xu_varg_list);                                                                                     \
        if (__xu_ret >= 0) s[__xu_ret] = '\0';                                                                         \
        *r = __xu_ret > 0 ? __xu_ret : 0;                                                                              \
                                                                                                                       \
    } while (0)

// vswprintf format
#define xu_vswprintf_format(s, n, format, r)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        xu_long_t    __xu_ret = 0;                                                                                     \
        xu_va_list_t __xu_varg_list;                                                                                   \
        xu_va_start(__xu_varg_list, format);                                                                           \
        __xu_ret = xu_vswprintf(s, (n), format, __xu_varg_list);                                                       \
        xu_va_end(__xu_varg_list);                                                                                     \
        if (__xu_ret >= 0) s[__xu_ret] = L'\0';                                                                        \
        *r = __xu_ret > 0 ? __xu_ret : 0;                                                                              \
                                                                                                                       \
    } while (0)

/* *******************************************************
 *  interface
 */
/*! returns the next character from the standard input (stdin).
 *
 * @returns          the next character or XU_EOF
 */
xu_int_t xu_getchar(xu_noarg_t);

/*! writes a character to the standard output (stdout).
 *
 * @returns          the character written is returned, or XU_EOF
 */
xu_int_t xu_putchar(xu_int_t ch);

/*! puts
 *
 * @param string    the string
 *
 * @return          the real size
 */
xu_long_t xu_puts(xu_char_t const* string);

/*! wputs
 *
 * @param string    the string
 *
 * @return          the real size
 */
xu_long_t xu_wputs(xu_wchar_t const* string);

/*! printf
 *
 * @param format    the format string
 *
 * @return          the real size
 *
 * - format: %[flags][width][.precision][qualifier]type
 *
 * - flags:
 *   - default: right-justified, left-pad the output with spaces until the required length of output is attained.
 *              If combined with '0' (see below),
 *              it will cause the sign to become a space when positive,
 *              but the remaining characters will be zero-padded
 *   - -:       left-justified, e.g. %-d
 *   - +:       denote the sign '+' or '-' of a number
 *   - 0:       use 0 instead of spaces to left-fill a fixed-length field
 *   - #:       add prefix or suffix
 *     - %#o => add prefix: 0...
 *     - %#x => add prefix: 0x...
 *     - %#X => add prefix: 0X...
 *     - %#b => add prefix: 0b...
 *     - %#B => add prefix: 0B...
 *     - %#f => add prefix: 0f...
 *     - %#F => add prefix: 0F...
 *
 * - width:
 *   - n:       n = 1, 2, 3, ..., fill spaces
 *   - 0n:      n = 1, 2, 3, ..., fill 0
 *   - *:       causes printf to pad the output until it is n characters wide,
 *              where n is an integer value stored in the a function argument just preceding
 *              that represented by the modified type.
 *              e.g. printf("%*d", 5, 10) will result in "10" being printed with a width of 5.
 *
 * - .precision:
 *   - .n:      for non-integral numeric types, causes the decimal portion of the output to be expressed in at least
 * number digits. for the string type, causes the output to be truncated at number characters. if the precision is zero,
 * nothing is printed for the corresponding argument.
 *   - *:       same as the above, but uses an integer value in the intoken argument to
 *              determine the number of decimal places or maximum string length.
 *              e.g. printf("%.*s", 3, "abcdef") will result in "abc" being printed.
 *
 * - qualifier:
 *   - h:       short integer or single double-point
 *   - l:       long integer or double double-point
 *   - I8:      8-bit integer
 *   - I16:     16-bit integer
 *   - I32:     32-bit integer
 *   - I64/ll:  64-bit integer
 *
 * @note support h, l, I8, I16, I32, I64, ll
 *
 * - type(e.g. %d %x %u %% ...):
 *   - d, i:    print an int as a signed decimal number.
 *              '%d' and '%i' are synonymous for output, but are different when used with scanf() for input.
 *   - u:       print decimal unsigned int.
 *   - o:       print an unsigned int in octal.
 *   - x/X:     print an unsigned int as a hexadecimal number. 'x' uses lower-case letters and 'X' uses upper-case.
 *   - b/B:     print an unsigned binary interger
 *   - e/E:     print a double value in standard form ([-]d.ddd e[+/-]ddd).
 *              An E conversion uses the letter E (rather than e) to introduce the exponent.
 *              The exponent always contains at least two digits; if the value is zero, the exponent is 00.
 *              e.g. 3.141593e+00
 *   - f/F:     Print a double in normal (fixed-point) notation.
 *              'f' and 'F' only differs in how the strings for an infinite number or NaN are printed
 *              ('inf', 'infinity' and 'nan' for 'f', 'INF', 'INFINITY' and 'NAN' for 'F').
 *   - g/G:     print a double in either normal or exponential notation, whichever is more appropriate for its
 * magnitude. 'g' uses lower-case letters, 'G' uses upper-case letters. This type differs slightly from fixed-point
 * notation in that insignificant zeroes to the right of the decimal point are not included. Also, the decimal point is
 * not included on whole numbers.
 *   - c:       print a char (character).
 *   - s:       print a character string
 *   - p:       print a void * (pointer to void) in an implementation-defined format.
 *   - n:       print nothing, but write number of characters successfully written so far into an integer pointer
 * parameter.
 *   - %:       %
 *
 * @note support        d, i, u, o, u, x/X, b/B, f/F, c, s
 * @note not support    e/E, g/G, p, n
 *
 * @code
 * xu_printf("|hello world|\n");
 * xu_printf("|%-10s|%%|%10s|\n", "hello", "world");
 * xu_printf("|%#2c|%2.5c|%*c|\n", 'A', 'B', 5, 'C');
 * xu_printf("|%#2d|%#8.3o|%*.*d|\n", -56, 56, 10, 5, 56);
 * xu_printf("|%#-8.5x|%#2.9X|\n", 0x1f, 0x1f);
 * xu_printf("|%#-8.5b|%#2.9B|\n", 0x1f, 0x1f);
 * xu_printf("|%-6Id|%5I8u|%#I64x|%#llx|\n", 256, 255, (xu_int64_t)0x8fffffffffff, (xu_int64_t)0x8fffffffffff);
 * xu_printf("|%lf|\n", -3.1415926535897932384626433832795);
 * xu_printf("|%lf|%lf|%lf|\n", 3.14, 0, -0);
 * xu_printf("|%0.9f|\n", 3.1415926535897932384626433832795);
 * xu_printf("|%16.9f|\n", 3.1415926535897932384626433832795);
 * xu_printf("|%016.9f|\n", 3.14159);
 * xu_printf("|%lf|\n", 1.0 / 6.0);
 * xu_printf("|%lf|\n", 0.0003141596);
 * xu_printf("|%F|\n", xu_float_to_fixed(3.1415));
 * xu_printf("|%{object_name}|\n", object);
 * @endcode
 */
xu_long_t xu_printf(xu_char_t const* format, ...);

/*! wprintf
 *
 * @param format    the format string
 *
 * @return          the real size
 */
xu_long_t xu_wprintf(xu_wchar_t const* format, ...);

/*! sprintf
 *
 * @param s         the string data
 * @param format    the format string
 *
 * @return          the real size
 */
xu_long_t xu_sprintf(xu_char_t* s, xu_char_t const* format, ...);

/*! snprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 *
 * @return          the real size
 */
xu_long_t xu_snprintf(xu_char_t* s, xu_size_t n, xu_char_t const* format, ...);

/*! vsnprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 * @param args      the arguments
 *
 * @return          the real size
 */
xu_long_t xu_vsnprintf(xu_char_t* s, xu_size_t n, xu_char_t const* format, xu_va_list_t args);

/*! swprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 *
 * @return          the real size
 */
xu_long_t xu_swprintf(xu_wchar_t* s, xu_size_t n, xu_wchar_t const* format, ...);

/*! vswprintf
 *
 * @param s         the string data
 * @param n         the string size
 * @param format    the format string
 * @param args      the arguments
 *
 * @return          the real size
 */
xu_long_t xu_vswprintf(xu_wchar_t* s, xu_size_t n, xu_wchar_t const* format, xu_va_list_t args);
