#include "xutil/unit_test/ut.h"

/* *******************************************************
 * implementation
 */
static xu_void_t xu_test_writ()
{
    xu_trace_i("=================== test stdfile writ ===================");
    xu_stdfile_writ(xu_stdfile_output(), (xu_byte_t const*)"hello world!\n", xu_strlen("hello world!\n"));
}
static xu_void_t xu_test_read()
{
    xu_trace_i("=================== test stdfile read ===================");
    xu_byte_t data[4096] = {0};
    if (xu_stdfile_read(xu_stdfile_input(), data, 3)) xu_trace_i("%s", data);
    xu_trace_i("------");

    // ignore left characters
    xu_char_t ch;
    while (xu_stdfile_getc(xu_stdfile_input(), &ch) && ch != '\n')
        ;
}
static xu_void_t xu_test_puts()
{
    xu_trace_i("=================== test stdfile puts ===================");
    xu_stdfile_puts(xu_stdfile_output(), "hello world!\n");
}
static xu_void_t xu_test_gets()
{
    xu_trace_i("=================== test stdfile gets ===================");
    xu_char_t data[4096] = {0};
    if (xu_stdfile_gets(xu_stdfile_input(), data, sizeof(data))) xu_trace_i("%s", data);
    xu_trace_i("------");
}
static xu_void_t xu_test_putc()
{
    xu_trace_i("=================== test stdfile putc ===================");
    xu_char_t const* s = "hello world!\n";
    xu_size_t        n = xu_strlen("hello world!\n");
    xu_size_t        i = 0;
    for (i = 0; i < n; i++)
        xu_stdfile_putc(xu_stdfile_output(), s[i]);
}
static xu_void_t xu_test_getc()
{
    xu_trace_i("=================== test stdfile getc ===================");
    xu_char_t ch;
    xu_int_t  i       = 0;
    xu_char_t s[4096] = {0};
    while (xu_stdfile_getc(xu_stdfile_input(), &ch) && ch != '\n')
        s[i++] = (xu_char_t)ch;
    s[i] = '\0';
    xu_trace_i("%s", s);
    xu_trace_i("------");
}
static xu_void_t xu_test_peek()
{
    xu_trace_i("=================== test stdfile peek ===================");
    xu_char_t ch;
    xu_int_t  i       = 0;
    xu_char_t s[4096] = {0};
    while (xu_stdfile_getc(xu_stdfile_input(), &ch) && ch != '\n')
    {
        xu_char_t ch2;
        if (xu_stdfile_peek(xu_stdfile_input(), &ch2)) xu_trace_i("peek: %c", ch2);
        s[i++] = (xu_char_t)ch;
    }
    s[i] = '\0';
    xu_trace_i("%s", s);
    xu_trace_i("------");
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_stdfile_main(xu_int_t argc, xu_char_t** argv)
{
    xu_test_writ();
    xu_test_putc();
    xu_test_puts();
    xu_test_read();
    xu_test_getc();
    xu_test_gets();
    xu_test_peek();
    return 0;
}
