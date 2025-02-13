#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_static_buffer_main(xu_int_t argc, xu_char_t** argv)
{
    xu_byte_t data[1024];

    xu_static_buffer_t b;
    xu_static_buffer_init(&b, data, 1024);

    xu_static_buffer_memncpy(&b, (xu_byte_t const*)"hello ", 6);
    xu_static_buffer_memncat(&b, (xu_byte_t const*)"world", 6);
    xu_trace_i("%s", xu_static_buffer_data(&b));

    xu_static_buffer_exit(&b);

    return 0;
}
