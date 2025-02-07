#include "xutil/platform/stdfile.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"
#include <stdio.h>
#include <stdlib.h>

/* *******************************************************
 * types
 */

// the stdfile type
typedef struct __xu_stdfile_t
{
    // the file type
    xu_size_t   type;

    // the file pointer
    FILE*       fp;

}xu_stdfile_t;

/* *******************************************************
 * implementation
 */
xu_stdfile_ref_t xu_stdfile_init(xu_size_t type)
{
    // check
    xu_assert_and_check_return_val(type, xu_null);

    // get std pointer
    FILE* fp = xu_null;
    switch (type)
    {
    case XU_STDFILE_TYPE_STDIN:  fp = stdin; break;
    case XU_STDFILE_TYPE_STDOUT: fp = stdout; break;
    case XU_STDFILE_TYPE_STDERR: fp = stderr; break;
    }
    xu_assert_and_check_return_val(fp, xu_null);

    // create standard device file
    xu_bool_t ok = xu_false;
    xu_stdfile_t* file = xu_null;
    do
    {
        // make file
        file = xu_malloc0_type(xu_stdfile_t);
        xu_assert_and_check_break(file);

        // init file
        file->fp    = fp;
        file->type  = type;

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit file
    if (!ok)
    {
        if (file) xu_stdfile_exit((xu_stdfile_ref_t)file);
        file = xu_null;
    }
    return (xu_stdfile_ref_t)file;
}
xu_void_t xu_stdfile_exit(xu_stdfile_ref_t self)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return(stdfile);

    // free it
    xu_free(stdfile);
}
xu_size_t xu_stdfile_type(xu_stdfile_ref_t self)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile, XU_STDFILE_TYPE_NONE);

    return stdfile->type;
}
xu_bool_t xu_stdfile_flush(xu_stdfile_ref_t self)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp, xu_false);
    xu_assert_and_check_return_val(stdfile->type != XU_STDFILE_TYPE_STDIN, xu_false);

    return !fflush(stdfile->fp)? xu_true : xu_false;
}
xu_bool_t xu_stdfile_read(xu_stdfile_ref_t self, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && data, xu_false);
    xu_assert_and_check_return_val(stdfile->type == XU_STDFILE_TYPE_STDIN, xu_false);

    // read data from stdin
    return fread(data, size, 1, stdfile->fp) == 1;
}
xu_bool_t xu_stdfile_writ(xu_stdfile_ref_t self, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && data, xu_false);
    xu_assert_and_check_return_val(stdfile->type != XU_STDFILE_TYPE_STDIN, xu_false);

    // write data to stdout/stderr
    return fwrite(data, size, 1, stdfile->fp) == 1;
}
#if defined(XU_CONFIG_LIBC_HAVE_FGETC) && defined(XU_CONFIG_LIBC_HAVE_UNGETC)
xu_bool_t xu_stdfile_peek(xu_stdfile_ref_t self, xu_char_t* pch)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && pch, xu_false);
    xu_assert_and_check_return_val(stdfile->type == XU_STDFILE_TYPE_STDIN, xu_false);

    // read character from stdin
    xu_int_t ch = fgetc(stdfile->fp);
    xu_check_return_val(ch != EOF, xu_false);

    // unread character from stdin
    ungetc(ch, stdfile->fp);

    // save result
    *pch = (xu_char_t)ch;
    return xu_true;
}
#else
xu_bool_t xu_stdfile_peek(xu_stdfile_ref_t self, xu_char_t* pch)
{
    xu_trace_noimpl();
    return xu_false;
}
#endif
#ifdef XU_CONFIG_LIBC_HAVE_FGETC
xu_bool_t xu_stdfile_getc(xu_stdfile_ref_t self, xu_char_t* pch)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && pch, xu_false);
    xu_assert_and_check_return_val(stdfile->type == XU_STDFILE_TYPE_STDIN, xu_false);

    // read character from stdin
    xu_int_t ch = fgetc(stdfile->fp);
    xu_check_return_val(ch != EOF, xu_false);

    // save result
    *pch = (xu_char_t)ch;
    return xu_true;
}
#else
xu_bool_t xu_stdfile_getc(xu_stdfile_ref_t self, xu_char_t* pch)
{
    return xu_stdfile_read(self, (xu_byte_t*)pch, 1);
}
#endif
#ifdef XU_CONFIG_LIBC_HAVE_FPUTC
xu_bool_t xu_stdfile_putc(xu_stdfile_ref_t self, xu_char_t ch)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp, xu_false);
    xu_assert_and_check_return_val(stdfile->type != XU_STDFILE_TYPE_STDIN, xu_false);

    // write character to stdout/stderr
    return fputc((xu_int_t)ch, stdfile->fp) == ch;
}
#else
xu_bool_t xu_stdfile_putc(xu_stdfile_ref_t self, xu_char_t ch)
{
    return xu_stdfile_writ(self, (xu_byte_t const*)&ch, 1);
}
#endif
#ifdef XU_CONFIG_LIBC_HAVE_FGETS
xu_bool_t xu_stdfile_gets(xu_stdfile_ref_t self, xu_char_t* str, xu_size_t num)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && str && num, xu_false);
    xu_assert_and_check_return_val(stdfile->type == XU_STDFILE_TYPE_STDIN, xu_false);

    // read string from stdin
    return fgets(str, (xu_int_t)num, stdfile->fp) == str;
}
#else
xu_bool_t xu_stdfile_gets(xu_stdfile_ref_t self, xu_char_t* str, xu_size_t num)
{
    // check
    xu_assert_and_check_return_val(num && str, xu_false);

    xu_size_t i;
    xu_char_t ch;
    for (i = 0; i < num - 1; i++)
    {
        if (xu_stdfile_getc(self, &ch))
        {
            if (ch == '\n')
            {
                str[i++] = '\n';
                break;
            }
            else str[i] = ch;
        }
        else return xu_false;
    }
    if (i < num) str[i] = '\0';
    return i < num;
}
#endif
#ifdef XU_CONFIG_LIBC_HAVE_FPUTS
xu_bool_t xu_stdfile_puts(xu_stdfile_ref_t self, xu_char_t const* str)
{
    // check
    xu_stdfile_t* stdfile = (xu_stdfile_t*)self;
    xu_assert_and_check_return_val(stdfile && stdfile->fp && str, xu_false);
    xu_assert_and_check_return_val(stdfile->type != XU_STDFILE_TYPE_STDIN, xu_false);

    // write string to stdout/stderr
    return fputs(str, stdfile->fp) >= 0;
}
#else
xu_bool_t xu_stdfile_puts(xu_stdfile_ref_t self, xu_char_t const* str)
{
    // check
    xu_assert_and_check_return_val(str, xu_false);

    // write string to stdout/stderr
    xu_size_t len = xu_strlen(str);
    return len? xu_stdfile_writ(self, (xu_byte_t const*)str, xu_strlen(str)) : xu_true;
}
#endif
