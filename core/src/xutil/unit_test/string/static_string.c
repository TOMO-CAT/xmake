#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_string_static_string_main(xu_int_t argc, xu_char_t** argv)
{
    xu_static_string_t s;
    xu_char_t          b[4096];
    xu_static_string_init(&s, b, 4096);

    xu_static_string_cstrcpy(&s, "hello");
    xu_static_string_chrcat(&s, ' ');
    xu_static_string_cstrfcat(&s, "%s", "world");
    xu_trace_i("%s", xu_static_string_cstr(&s));

    xu_static_string_exit(&s);

    return 0;
}
