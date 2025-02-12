#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define XU_TEST_CMP (1)
#define XU_TEST_LEN (1)
#define XU_TEST_CPY (1)

/* *******************************************************
 * compare
 */
static xu_void_t xu_test_strcmp(xu_char_t const* s1, xu_char_t const* s2)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_strcmp(s1, s2);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strcmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static xu_void_t xu_test_strncmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t size)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_strncmp(s1, s2, size);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strncmp(%s, %s, %u) = %ld\n", t, s1, s2, size, r);
}
static xu_void_t xu_test_stricmp(xu_char_t const* s1, xu_char_t const* s2)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_stricmp(s1, s2);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_stricmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static xu_void_t xu_test_strnicmp(xu_char_t const* s1, xu_char_t const* s2, xu_size_t size)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_strnicmp(s1, s2, size);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strnicmp(%s, %s, %u) = %ld\n", t, s1, s2, size, r);
}
static xu_void_t xu_test_strlen(xu_char_t const* s)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_strlen(s);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strlen(%s) = %ld\n", t, s, r);
}
static xu_void_t xu_test_strnlen(xu_char_t const* s, xu_size_t size)
{
    __xu_volatile__ xu_long_t n = 1000000;
    __xu_volatile__ xu_long_t r = 0;
    xu_hong_t                 t = xu_mclock();
    while (n--)
    {
        r = xu_strnlen(s, size);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strnlen(%s, %u) = %ld\n", t, s, size, r);
}
static xu_void_t xu_test_strcpy(xu_char_t const* s2)
{
    __xu_volatile__ xu_int_t n = 1000000;
    xu_char_t                s1[4096];
    xu_hong_t                t = xu_mclock();
    while (n--)
    {
        xu_strcpy(s1, s2);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strcpy(%s) = %s\n", t, s2, s1);
}
static xu_void_t xu_test_strncpy(xu_char_t const* s2, xu_size_t size)
{
    __xu_volatile__ xu_int_t n = 1000000;
    xu_char_t                s1[4096];
    xu_hong_t                t = xu_mclock();
    while (n--)
    {
        xu_strlcpy(s1, s2, size);
    }
    t = xu_mclock() - t;
    xu_printf("%lld ms, xu_test_strncpy(%s, %d) = %s\n", t, s2, size, s1);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_libc_string_main(xu_int_t argc, xu_char_t** argv)
{
#if XU_TEST_CMP
    xu_printf("=================================================================\n");
    xu_test_strcmp("", "");
    xu_test_strcmp("1", "1");
    xu_test_strcmp("1234567890", "1234567890");
    xu_test_strcmp("1234567890abcbefg", "1234567890ABCBEFG");
    xu_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    xu_test_strcmp("1234", "1234567890");
    xu_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    xu_printf("\n");
    xu_test_stricmp("", "");
    xu_test_stricmp("1", "1");
    xu_test_stricmp("1234567890", "1234567890");
    xu_test_stricmp("1234567890abcbefg", "1234567890ABCBEFG");
    xu_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    xu_test_stricmp("1234", "1234567890");
    xu_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    xu_printf("\n");
    xu_test_strncmp("", "", 10);
    xu_test_strncmp("1", "1", 10);
    xu_test_strncmp("1234567890", "1234567890", 10);
    xu_test_strncmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    xu_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    xu_test_strncmp("1234", "1234567890", 10);
    xu_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

    xu_printf("\n");
    xu_test_strnicmp("", "", 10);
    xu_test_strnicmp("1", "1", 10);
    xu_test_strnicmp("1234567890", "1234567890", 10);
    xu_test_strnicmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    xu_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    xu_test_strnicmp("1234", "1234567890", 10);
    xu_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

#endif

#if XU_TEST_LEN
    xu_printf("=================================================================\n");
    xu_test_strlen("");
    xu_test_strlen("1");
    xu_test_strlen("1234567890");
    xu_test_strlen("1234567890abcbefg");
    xu_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890");
    xu_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1"
                   "234567890abcdefghijklmnopqrstuvwxyz1234567890");

    xu_printf("\n");
    xu_test_strnlen("", 10);
    xu_test_strnlen("1", 10);
    xu_test_strnlen("1234567890", 10);
    xu_test_strnlen("1234567890abcbefg", 20);
    xu_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890", 20);
    xu_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz"
                    "1234567890abcdefghijklmnopqrstuvwxyz1234567890",
                    60);

#endif

#if XU_TEST_CPY
    xu_printf("=================================================================\n");
    xu_test_strcpy("");
    xu_test_strcpy("1");
    xu_test_strcpy("1234567890");
    xu_test_strcpy("1234567890abcbefg");
    xu_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890");
    xu_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    xu_printf("\n");
    xu_test_strncpy("", 5);
    xu_test_strncpy("1", 5);
    xu_test_strncpy("1234567890", 5);
    xu_test_strncpy("1234567890abcbefg", 5);
    xu_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890", 5);
    xu_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 50);

#endif

    return 0;
}
