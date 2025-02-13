#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_string_string_main(xu_int_t argc, xu_char_t** argv)
{
    xu_string_t s;
    xu_string_init(&s);

    xu_string_cstrcpy(&s, "hello");
    xu_string_chrcat(&s, ' ');
    xu_string_cstrfcat(&s, "%s", "world");
    xu_string_chrcat(&s, ' ');
    xu_string_chrncat(&s, 'x', 5);
    xu_trace_i("%d: %s", xu_string_size(&s), xu_string_cstr(&s));

    xu_string_exit(&s);

    return 0;
}
