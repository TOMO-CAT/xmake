#include "xutil/memory/static_buffer.h"
#include "xutil/libc/string/string.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the maximum grow size of value buffer
#ifdef __xu_small__
#    define XU_STATIC_BUFFER_GROW_SIZE (64)
#else
#    define XU_STATIC_BUFFER_GROW_SIZE (256)
#endif

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_static_buffer_init(xu_static_buffer_ref_t buffer, xu_byte_t* data, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_false);

    // init
    buffer->size = 0;
    buffer->data = data;
    buffer->maxn = maxn;

    // ok
    return xu_true;
}
xu_void_t xu_static_buffer_exit(xu_static_buffer_ref_t buffer)
{
    // exit it
    if (buffer)
    {
        buffer->data = xu_null;
        buffer->size = 0;
        buffer->maxn = 0;
    }
}
xu_byte_t* xu_static_buffer_data(xu_static_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // the buffer data
    return buffer->data;
}
xu_size_t xu_static_buffer_size(xu_static_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the buffer size
    return buffer->size;
}
xu_size_t xu_static_buffer_maxn(xu_static_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the buffer maxn
    return buffer->maxn;
}
xu_void_t xu_static_buffer_clear(xu_static_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return(buffer);

    // clear it
    buffer->size = 0;
}
xu_byte_t* xu_static_buffer_resize(xu_static_buffer_ref_t buffer, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(buffer && buffer->data && size <= buffer->maxn, xu_null);

    // resize
    buffer->size = size;

    // ok
    return buffer->data;
}
xu_byte_t* xu_static_buffer_memset(xu_static_buffer_ref_t buffer, xu_byte_t b)
{
    return xu_static_buffer_memnsetp(buffer, 0, b, xu_static_buffer_size(buffer));
}
xu_byte_t* xu_static_buffer_memsetp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_byte_t b)
{
    return xu_static_buffer_memnsetp(buffer, p, b, xu_static_buffer_size(buffer));
}
xu_byte_t* xu_static_buffer_memnset(xu_static_buffer_ref_t buffer, xu_byte_t b, xu_size_t n)
{
    return xu_static_buffer_memnsetp(buffer, 0, b, n);
}
xu_byte_t* xu_static_buffer_memnsetp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_byte_t b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // check
    xu_check_return_val(n, xu_static_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_static_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memset
    xu_memset(d + p, b, n);

    // ok?
    return d;
}
xu_byte_t* xu_static_buffer_memcpy(xu_static_buffer_ref_t buffer, xu_static_buffer_ref_t b)
{
    return xu_static_buffer_memncpyp(buffer, 0, xu_static_buffer_data(b), xu_static_buffer_size(b));
}
xu_byte_t* xu_static_buffer_memcpyp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_static_buffer_ref_t b)
{
    return xu_static_buffer_memncpyp(buffer, p, xu_static_buffer_data(b), xu_static_buffer_size(b));
}
xu_byte_t* xu_static_buffer_memncpy(xu_static_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n)
{
    return xu_static_buffer_memncpyp(buffer, 0, b, n);
}
xu_byte_t* xu_static_buffer_memncpyp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_byte_t const* b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && b, xu_null);

    // check
    xu_check_return_val(n, xu_static_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_static_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memcpy
    xu_memcpy(d + p, b, n);

    // ok?
    return d;
}
xu_byte_t* xu_static_buffer_memmov(xu_static_buffer_ref_t buffer, xu_size_t b)
{
    // check
    xu_assert_and_check_return_val(b <= xu_static_buffer_size(buffer), xu_null);
    return xu_static_buffer_memnmovp(buffer, 0, b, xu_static_buffer_size(buffer) - b);
}
xu_byte_t* xu_static_buffer_memmovp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_size_t b)
{
    // check
    xu_assert_and_check_return_val(b <= xu_static_buffer_size(buffer), xu_null);
    return xu_static_buffer_memnmovp(buffer, p, b, xu_static_buffer_size(buffer) - b);
}
xu_byte_t* xu_static_buffer_memnmov(xu_static_buffer_ref_t buffer, xu_size_t b, xu_size_t n)
{
    return xu_static_buffer_memnmovp(buffer, 0, b, n);
}
xu_byte_t* xu_static_buffer_memnmovp(xu_static_buffer_ref_t buffer, xu_size_t p, xu_size_t b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && (b + n) <= xu_static_buffer_size(buffer), xu_null);

    // clear?
    if (b == xu_static_buffer_size(buffer))
    {
        xu_static_buffer_clear(buffer);
        return xu_static_buffer_data(buffer);
    }

    // check
    xu_check_return_val(p != b && n, xu_static_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_static_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memmov
    xu_memmov(d + p, d + b, n);

    // ok?
    return d;
}
xu_byte_t* xu_static_buffer_memcat(xu_static_buffer_ref_t buffer, xu_static_buffer_ref_t b)
{
    return xu_static_buffer_memncat(buffer, xu_static_buffer_data(b), xu_static_buffer_size(b));
}
xu_byte_t* xu_static_buffer_memncat(xu_static_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && b, xu_null);

    // check
    xu_check_return_val(n, xu_static_buffer_data(buffer));

    // is null?
    xu_size_t p = xu_static_buffer_size(buffer);
    if (!p) return xu_static_buffer_memncpy(buffer, b, n);

    // resize
    xu_byte_t* d = xu_static_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memcat
    xu_memcpy(d + p, b, n);

    // ok?
    return d;
}
