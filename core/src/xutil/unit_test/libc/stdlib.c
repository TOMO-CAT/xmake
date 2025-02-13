#include "xutil/unit_test/ut.h"
#include <ctype.h>

/* *******************************************************
 * macros
 */
#define XU_TEST_ISXXX (0)

#define XU_TEST_TOLOWER (0)
#define XU_TEST_TOUPPER (0)

#define XU_TEST_SBTOI32 (1)
#define XU_TEST_SBTOU32 (1)
#define XU_TEST_SBTOF (1)

#define XU_TEST_STOI32 (1)
#define XU_TEST_STOU32 (1)

/* *******************************************************
 * implementation
 */
#if XU_TEST_ISXXX
static xu_void_t xu_check_is()
{
    xu_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((xu_isspace(i) ? 1 : 0) != (isspace(i) ? 1 : 0)) xu_printf("[e] isspace: 0x%02x\n", i);
        if ((xu_isalpha(i) ? 1 : 0) != (isalpha(i) ? 1 : 0)) xu_printf("[e] isalpha: 0x%02x\n", i);
        if ((xu_isdigit(i) ? 1 : 0) != (isdigit(i) ? 1 : 0)) xu_printf("[e] isdigit: 0x%02x\n", i);
        if ((xu_isupper(i) ? 1 : 0) != (isupper(i) ? 1 : 0)) xu_printf("[e] isupper: 0x%02x\n", i);
        if ((xu_islower(i) ? 1 : 0) != (islower(i) ? 1 : 0)) xu_printf("[e] islower: 0x%02x\n", i);
        if ((xu_isascii(i) ? 1 : 0) != (isascii(i) ? 1 : 0)) xu_printf("[e] isascii: 0x%02x\n", i);
    }
}
#endif
static xu_void_t xu_make_isspace_table()
{
    xu_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if (isspace(i)) xu_printf("0x%02x\n", i);
    }
}
static xu_void_t xu_make_isalpha_table()
{
    xu_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if (isalpha(i)) xu_printf("0x%02x\n", i);
    }
}
#if XU_TEST_TOUPPER
static xu_void_t xu_check_toupper()
{
    xu_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((xu_toupper(i) ? 1 : 0) != (toupper(i) ? 1 : 0)) xu_printf("[e] toupper: 0x%02x = 0x%02x\n", i, toupper(i));
    }
}
#endif
#if XU_TEST_TOLOWER
static xu_void_t xu_check_tolower()
{
    xu_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((xu_tolower(i) ? 1 : 0) != (tolower(i) ? 1 : 0)) xu_printf("[e] tolower: 0x%02x = 0x%02x\n", i, tolower(i));
    }
}
#endif
static xu_void_t xu_test_sbtou32(xu_char_t const* s, xu_int_t base, xu_uint32_t val)
{
    xu_printf("s%dtou32(%s) = %u [?= %u]\n", base, s, xu_sbtou32(s, base), val);
}
static xu_void_t xu_test_sbtoi32(xu_char_t const* s, xu_int_t base, xu_int32_t val)
{
    xu_printf("s%dtoi32(%s) = %d [?= %d]\n", base, s, xu_sbtoi32(s, base), val);
}

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
static xu_void_t xu_test_sbtof(xu_char_t const* s, xu_int_t base, xu_double_t val)
{
    xu_printf("s%dtof(%s) = %lf [?= %lf]\n", base, s, xu_sbtof(s, base), val);
}
#endif
static xu_void_t xu_test_stou32(xu_char_t const* s, xu_uint32_t val)
{
    xu_printf("stou32(%s) = %u [?= %u]\n", s, xu_stou32(s), val);
}
static xu_void_t xu_test_stoi32(xu_char_t const* s, xu_int32_t val)
{
    xu_printf("stoi32(%s) = %d [?= %d]\n", s, xu_stoi32(s), val);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_libc_stdlib_main(xu_int_t argc, xu_char_t** argv)
{
    xu_make_isspace_table();
    xu_make_isalpha_table();

#if XU_TEST_ISXXX
    xu_printf("===============================\n");
    xu_check_is();
#endif

#if XU_TEST_TOLOWER
    xu_printf("===============================\n");
    xu_check_tolower();
#endif

#if XU_TEST_TOUPPER
    xu_printf("===============================\n");
    xu_check_toupper();
#endif

#if XU_TEST_SBTOU32
    xu_printf("===============================\n");
    xu_test_sbtou32("1110111001100011001100110", 2, 31415926);
    xu_test_sbtou32("0b1110111001100011001100110", 2, 31415926);
    xu_test_sbtou32("167143146", 8, 31415926);
    xu_test_sbtou32("0167143146", 8, 31415926);
    xu_test_sbtou32("31415926", 10, 31415926);
    xu_test_sbtou32("1dcc666", 16, 31415926);
    xu_test_sbtou32("0x1dcc666", 16, 31415926);
#endif

#if XU_TEST_SBTOI32
    xu_printf("===============================\n");
    xu_test_sbtoi32("1110111001100011001100110", 2, 31415926);
    xu_test_sbtoi32("0b1110111001100011001100110", 2, 31415926);
    xu_test_sbtoi32("167143146", 8, 31415926);
    xu_test_sbtoi32("0167143146", 8, 31415926);
    xu_test_sbtoi32("31415926", 10, 31415926);
    xu_test_sbtoi32("1dcc666", 16, 31415926);
    xu_test_sbtoi32("0x1dcc666", 16, 31415926);

    xu_printf("\n");
    xu_test_sbtoi32("-1110111001100011001100110", 2, -31415926);
    xu_test_sbtoi32("-0b1110111001100011001100110", 2, -31415926);
    xu_test_sbtoi32("-167143146", 8, -31415926);
    xu_test_sbtoi32("-0167143146", 8, -31415926);
    xu_test_sbtoi32("-31415926", 10, -31415926);
    xu_test_sbtoi32("-1dcc666", 16, -31415926);
    xu_test_sbtoi32("-0x1dcc666", 16, -31415926);
#endif

#if XU_TEST_SBTOF && defined(XU_CONFIG_TYPE_HAVE_FLOAT)
    xu_printf("===============================\n");
    xu_test_sbtof("0", 10, 0);
    xu_test_sbtof("0.", 10, 0.);
    xu_test_sbtof("0.0", 10, 0.0);
    xu_test_sbtof("00.00", 10, 00.00);
    xu_test_sbtof("3", 10, 3);
    xu_test_sbtof("3.", 10, 3.);
    xu_test_sbtof("3.1415926", 10, 3.1415926);
    xu_test_sbtof("0.1415926", 10, 0.1415926);
    xu_test_sbtof("3.123456789123456789", 10, 3.123456789123456789);
    xu_test_sbtof("00003.0001415926000", 10, 00003.0001415926000);
    xu_test_sbtof("4294967295", 10, 4294967295ul);
    xu_test_sbtof("4294967295.", 10, 4294967295.);
    xu_test_sbtof("01234567", 8, 01234567);
    xu_test_sbtof("0xb14ac01c", 16, 0xb14ac01c);

    xu_printf("\n");
    xu_test_sbtof("-0", 10, -0);
    xu_test_sbtof("-0.", 10, -0.);
    xu_test_sbtof("-0.0", 10, -0.0);
    xu_test_sbtof("-00.00", 10, -00.00);
    xu_test_sbtof("-3", 10, -3);
    xu_test_sbtof("-3.", 10, -3.);
    xu_test_sbtof("-3.1415926", 10, -3.1415926);
    xu_test_sbtof("-0.1415926", 10, -0.1415926);
    xu_test_sbtof("-3.123456789123456789", 10, -3.123456789123456789);
    xu_test_sbtof("-00003.0001415926000", 10, -00003.0001415926000);
    xu_test_sbtof("-4294967295.", 10, -4294967295.f);

    xu_printf("===============================\n");
    xu_test_sbtof("0", 2, 0);
    xu_test_sbtof("0.", 2, 0.);
    xu_test_sbtof("0.0", 2, 0.0);
    xu_test_sbtof("00.00", 2, 00.00);
    xu_test_sbtof("11", 2, 3);
    xu_test_sbtof("11.", 2, 3.);
    xu_test_sbtof("11.001001", 2, 3.140625);
    xu_test_sbtof("0.001001", 2, 0.140625);
    xu_test_sbtof("11111111111111111111111111111111.", 2, 4294967295.);
#endif

#if XU_TEST_STOU32
    xu_printf("===============================\n");
    xu_test_stou32("0b1110111001100011001100110", 31415926);
    xu_test_stou32("0167143146", 31415926);
    xu_test_stou32("31415926", 31415926);
    xu_test_stou32("0x1dcc666", 31415926);
#endif

#if XU_TEST_STOI32
    xu_printf("===============================\n");
    xu_test_stoi32("0b1110111001100011001100110", 31415926);
    xu_test_stoi32("0167143146", 31415926);
    xu_test_stoi32("31415926", 31415926);
    xu_test_stoi32("0x1dcc666", 31415926);

    xu_printf("\n");
    xu_test_stoi32("-0b1110111001100011001100110", -31415926);
    xu_test_stoi32("-0167143146", -31415926);
    xu_test_stoi32("-31415926", -31415926);
    xu_test_stoi32("-0x1dcc666", -31415926);
#endif
    return 0;
}
