#pragma once

#include "xutil/libc/misc/ctype.h"
#include "xutil/libm/inf.h"
#include "xutil/libm/nan.h"
#include "xutil/macros/assert.h"
#include "xutil/prefix.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 *  macros
 * *******************************************************
 */
// for uint32
#define xu_s2tou32(s) ((xu_uint32_t)xu_s2tou64(s))
#define xu_s8tou32(s) ((xu_uint32_t)xu_s8tou64(s))
#define xu_s10tou32(s) ((xu_uint32_t)xu_s10tou64(s))
#define xu_s16tou32(s) ((xu_uint32_t)xu_s16tou64(s))
#define xu_stou32(s) ((xu_uint32_t)xu_stou64(s))
#define xu_sbtou32(s, b) ((xu_uint32_t)xu_sbtou64(s, b))

// for int32
#define xu_s2toi32(s) ((xu_int32_t)xu_s2tou64(s))
#define xu_s8toi32(s) ((xu_int32_t)xu_s8tou64(s))
#define xu_s10toi32(s) ((xu_int32_t)xu_s10tou64(s))
#define xu_s16toi32(s) ((xu_int32_t)xu_s16tou64(s))
#define xu_stoi32(s) ((xu_int32_t)xu_stou64(s))
#define xu_sbtoi32(s, b) ((xu_int32_t)xu_sbtou64(s, b))

// for int64
#define xu_s2toi64(s) ((xu_int64_t)xu_s2tou64(s))
#define xu_s8toi64(s) ((xu_int64_t)xu_s8tou64(s))
#define xu_s10toi64(s) ((xu_int64_t)xu_s10tou64(s))
#define xu_s16toi64(s) ((xu_int64_t)xu_s16tou64(s))
#define xu_stoi64(s) ((xu_int64_t)xu_stou64(s))
#define xu_sbtoi64(s, b) ((xu_int64_t)xu_sbtou64(s, b))

// for float
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    define xu_s2tof(s) ((xu_float_t)xu_s2tod(s))
#    define xu_s8tof(s) ((xu_float_t)xu_s8tod(s))
#    define xu_s10tof(s) ((xu_float_t)xu_s10tod(s))
#    define xu_s16tof(s) ((xu_float_t)xu_s16tod(s))
#    define xu_stof(s) ((xu_float_t)xu_stod(s))
#    define xu_sbtof(s, b) ((xu_float_t)xu_sbtod(s, b))
#endif

// for porting libc
#define xu_atoi(s) xu_s10toi32(s)
#define xu_atoll(s) xu_s10toi64(s)
#define xu_strtol(s, e, b) xu_sbtoi32(s, b)
#define xu_strtoll(s, e, b) xu_sbtoi64(s, b)
#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    define xu_atof(s) xu_s10tod(s)
#    define xu_strtof(s, e) xu_s10tof(s)
#    define xu_strtod(s, e) xu_s10tod(s)
#endif
#if XU_CPU_BIT64
#    define xu_atol(s) (xu_long_t) xu_s10toi64(s)
#else
#    define xu_atol(s) (xu_long_t) xu_s10toi32(s)
#endif

// atow
#define xu_atow(s1, s2, n) xu_mbstowcs(s1, s2, n)

// wtoa
#define xu_wtoa(s1, s2, n) xu_wcstombs(s1, s2, n)

// rand
#define xu_rand() (xu_int_t) xu_random()
#define xu_srand(seed) xu_srandom(seed)

/* *******************************************************
 * interfaces
 * *******************************************************
 */
/*! convert the binary string to uint64
 *
 * <pre>
 * .e.g
 *
 * "1001" => 9
 * "0b1001" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_s2tou64(xu_char_t const* s);

/*! convert the octal string to uint64
 *
 * <pre>
 * .e.g
 *
 * "11" => 9
 * "011" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_s8tou64(xu_char_t const* s);

/*! convert the decimal string to uint64
 *
 * .e.g "9" => 9
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_s10tou64(xu_char_t const* s);

/*! convert the hex string to uint64
 *
 * <pre>
 * .e.g
 *
 * "9" => 9
 * "0x9" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_s16tou64(xu_char_t const* s);

/*! auto convert string to uint64
 *
 * <pre>
 * .e.g
 *
 * "0b1001" => 9
 * "011"    => 9
 * "9"      => 9
 * "0x9"    => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_stou64(xu_char_t const* s);

/*! convert string to uint64 using the given base number
 *
 * @param s         the string
 *
 * @return          the uint64 number
 */
xu_uint64_t xu_sbtou64(xu_char_t const* s, xu_int_t base);

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

/*! convert the binary string to double
 *
 * <pre>
 * .e.g
 *
 * "1001" => 9
 * "0b1001" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_s2tod(xu_char_t const* s);

/*! convert the binary string to double
 *
 * <pre>
 * .e.g
 *
 * "11" => 9
 * "011" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_s8tod(xu_char_t const* s);

/*! convert the decimal string to double
 *
 * .e.g "9" => 9
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_s10tod(xu_char_t const* s);

/*! convert the hex string to double
 *
 * <pre>
 * .e.g
 *
 * "9" => 9
 * "0x9" => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_s16tod(xu_char_t const* s);

/*! auto convert string to double
 *
 * <pre>
 * .e.g
 *
 * "0b1001" => 9
 * "011"    => 9
 * "9"      => 9
 * "0x9"    => 9
 * </pre>
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_stod(xu_char_t const* s);

/*! convert string to double using the given base number
 *
 * @param s         the string
 *
 * @return          the double number
 */
xu_double_t xu_sbtod(xu_char_t const* s, xu_int_t base);

#endif

/*! mbstowcs, convert string to wstring
 *
 * @param s1        the wstring data
 * @param s2        the string data
 * @param n         the string length
 *
 * @return          the wstring length
 */
xu_size_t xu_mbstowcs(xu_wchar_t* s1, xu_char_t const* s2, xu_size_t n);

/*! wcstombs, convert wstring to string
 *
 * @param s1        the string data
 * @param s2        the wstring data
 * @param n         the wstring length
 *
 * @return          the string length
 */
xu_size_t xu_wcstombs(xu_char_t* s1, xu_wchar_t const* s2, xu_size_t n);

/*! update random seed
 *
 * @param seed      the random seed
 */
xu_void_t xu_srandom(xu_size_t seed);

/*! generate the random with range: [0, max)
 *
 * @return          the random value
 */
xu_long_t xu_random(xu_noarg_t);
