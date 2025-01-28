#include "xutil/unit_test/ut.h"
#include "xutil/utils/trace.h"

/* *******************************************************
 * test
 */
xu_void_t xu_demo_overflow(xu_noarg_t);
xu_void_t xu_demo_overflow()
{
    xu_pointer_t data = xu_malloc0(10);
    if (data)
    {
        xu_memset(data, 0, 11);
        xu_free(data);
    }
}
xu_void_t xu_demo_cstring(xu_noarg_t);
xu_void_t xu_demo_cstring()
{
    xu_char_t* data = xu_malloc0_cstr(10);
    if (data)
    {
        xu_memset(data, 'c', 10);
        xu_strlen(data);
        xu_free(data);
    }
}
xu_void_t xu_demo_free2(xu_noarg_t);
xu_void_t xu_demo_free2()
{
    xu_pointer_t data = xu_malloc0(10);
    if (data)
    {
        xu_free(data);
        xu_free(data);
    }
}
xu_void_t xu_demo_leak(xu_noarg_t);
xu_void_t xu_demo_leak()
{
    xu_pointer_t data  = xu_malloc0(100);
    xu_pointer_t data2 = xu_malloc0(10000);
    xu_used(data);
    xu_used(data2);
}
xu_void_t xu_demo_overlap(xu_noarg_t);
xu_void_t xu_demo_overlap()
{
    xu_pointer_t data = xu_malloc(10);
    if (data)
    {
        xu_memcpy(data, (xu_byte_t const*)data + 1, 5);
        xu_free(data);
    }
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_memory_check_main(xu_int_t argc, xu_char_t** argv)
{
    // debug 模式下可以进行各种内存检测
    // 详见: docs/xutil/memory/memory-check.md
    //
    // xu_demo_leak();
    // xu_demo_free2();
    // xu_demo_overflow();
    // xu_demo_cstring();
    // xu_demo_overlap();
    xu_trace_i("This unit test will never exit normally in debug mode, so we skip it");

    return 0;
}
