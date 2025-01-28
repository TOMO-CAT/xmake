#include "xutil/string/string.h"
#include "xutil/unit_test/ut.h"

xu_int_t xu_ut_memory_buffer_main(xu_int_t argc, xu_char_t** argv)
{
    xu_buffer_t b;
    xu_buffer_init(&b);

    xu_buffer_memncpy(&b, (xu_byte_t const*)"hello ", 6);
    xu_buffer_memncat(&b, (xu_byte_t const*)"world", 6);
    xu_trace_i("%s", xu_buffer_data(&b));

    xu_buffer_exit(&b);

    return 0;
}
