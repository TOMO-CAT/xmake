#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_queue_buffer_main(xu_int_t argc, xu_char_t** argv)
{
    xu_char_t         d[1024];
    xu_queue_buffer_t b;
    xu_queue_buffer_init(&b, 1024);

    xu_queue_buffer_writ(&b, (xu_byte_t const*)"hello", 5);
    xu_queue_buffer_writ(&b, (xu_byte_t const*)" ", 1);
    xu_queue_buffer_writ(&b, (xu_byte_t const*)"world", 6);
    xu_queue_buffer_read(&b, (xu_byte_t*)d, 1024);
    xu_trace_i("%s", d);

    xu_queue_buffer_exit(&b);

    return 0;
}
