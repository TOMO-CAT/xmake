#include "xutil/memory/buffer.h"

/* *******************************************************
 *  macros
 */

// the maximum grow size of value buffer
#ifdef __xu_small__
#    define XU_BUFFER_GROW_SIZE (64)
#else
#    define XU_BUFFER_GROW_SIZE (256)
#endif

/* *******************************************************
 *  implementation
 */

xu_bool_t xu_buffer_init(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_false);

    // init
    buffer->data = buffer->buff;
    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);

    // ok
    return xu_true;
}
xu_void_t xu_buffer_exit(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return(buffer);

    // clear it
    xu_buffer_clear(buffer);

    // exit data
    if (buffer->data && buffer->data != buffer->buff) xu_free(buffer->data);
    buffer->data = buffer->buff;

    // exit size
    buffer->size = 0;
    buffer->maxn = sizeof(buffer->buff);
}
xu_byte_t* xu_buffer_data(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // the buffer data
    return buffer->data;
}
xu_size_t xu_buffer_size(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the buffer size
    return buffer->size;
}
xu_size_t xu_buffer_maxn(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the buffer maxn
    return buffer->maxn;
}
xu_void_t xu_buffer_clear(xu_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return(buffer);

    // clear it
    buffer->size = 0;
}
xu_byte_t* xu_buffer_resize(xu_buffer_ref_t buffer, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(buffer && size, xu_null);

    // done
    xu_bool_t  ok        = xu_false;
    xu_byte_t* buff_data = buffer->data;
    xu_size_t  buff_size = buffer->size;
    xu_size_t  buff_maxn = buffer->maxn;
    do
    {
        // check
        xu_assert_and_check_break(buff_data);

        // using static buffer?
        if (buff_data == buffer->buff)
        {
            // grow?
            if (size > buff_maxn)
            {
                // grow maxn
                buff_maxn = xu_align8(size + XU_BUFFER_GROW_SIZE);
                xu_assert_and_check_break(size <= buff_maxn);

                // grow data
                buff_data = xu_malloc_bytes(buff_maxn);
                xu_assert_and_check_break(buff_data);

                // copy data
                xu_memcpy(buff_data, buffer->buff, buff_size);
            }

            // update the size
            buff_size = size;
        }
        else
        {
            // grow?
            if (size > buff_maxn)
            {
                // grow maxn
                buff_maxn = xu_align8(size + XU_BUFFER_GROW_SIZE);
                xu_assert_and_check_break(size <= buff_maxn);

                // grow data
                buff_data = (xu_byte_t*)xu_ralloc(buff_data, buff_maxn);
                xu_assert_and_check_break(buff_data);
            }
#if 0
            // decrease to the static buffer
            else if (size <= sizeof(buffer->buff))
            {
                // update the maxn
                buff_maxn = sizeof(buffer->buff);

                // copy data
                xu_memcpy(buffer->buff, buff_data, size);

                // free data
                xu_free(buff_data);

                // using the static buffer
                buff_data = buffer->buff;
            }
#endif

            // update the size
            buff_size = size;
        }

        // update the buffer
        buffer->data = buff_data;
        buffer->size = buff_size;
        buffer->maxn = buff_maxn;

        // ok
        ok = xu_true;

    } while (0);

    // trace
    xu_assertf(ok, "resize buffer failed: %lu => %lu", buff_size, size);

    // ok
    return ok ? (xu_byte_t*)buffer->data : xu_null;
}
xu_byte_t* xu_buffer_memset(xu_buffer_ref_t buffer, xu_byte_t b)
{
    return xu_buffer_memnsetp(buffer, 0, b, xu_buffer_size(buffer));
}
xu_byte_t* xu_buffer_memsetp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t b)
{
    xu_size_t size = xu_buffer_size(buffer);
    xu_assert_and_check_return_val(p < size, xu_null);
    return xu_buffer_memnsetp(buffer, p, b, size - p);
}
xu_byte_t* xu_buffer_memnset(xu_buffer_ref_t buffer, xu_byte_t b, xu_size_t n)
{
    return xu_buffer_memnsetp(buffer, 0, b, n);
}
xu_byte_t* xu_buffer_memnsetp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // check
    xu_check_return_val(n, xu_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memset
    xu_memset(d + p, b, n);

    // ok?
    return d;
}
xu_byte_t* xu_buffer_memcpy(xu_buffer_ref_t buffer, xu_buffer_ref_t b)
{
    return xu_buffer_memncpyp(buffer, 0, xu_buffer_data(b), xu_buffer_size(b));
}
xu_byte_t* xu_buffer_memcpyp(xu_buffer_ref_t buffer, xu_size_t p, xu_buffer_ref_t b)
{
    return xu_buffer_memncpyp(buffer, p, xu_buffer_data(b), xu_buffer_size(b));
}
xu_byte_t* xu_buffer_memncpy(xu_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n)
{
    return xu_buffer_memncpyp(buffer, 0, b, n);
}
xu_byte_t* xu_buffer_memncpyp(xu_buffer_ref_t buffer, xu_size_t p, xu_byte_t const* b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && b, xu_null);

    // check
    xu_check_return_val(n, xu_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // copy it
    xu_memcpy(d + p, b, n);

    // ok
    return d;
}
xu_byte_t* xu_buffer_memmov(xu_buffer_ref_t buffer, xu_size_t b)
{
    // check
    xu_assert_and_check_return_val(b <= xu_buffer_size(buffer), xu_null);
    return xu_buffer_memnmovp(buffer, 0, b, xu_buffer_size(buffer) - b);
}
xu_byte_t* xu_buffer_memmovp(xu_buffer_ref_t buffer, xu_size_t p, xu_size_t b)
{
    // check
    xu_assert_and_check_return_val(b <= xu_buffer_size(buffer), xu_null);
    return xu_buffer_memnmovp(buffer, p, b, xu_buffer_size(buffer) - b);
}
xu_byte_t* xu_buffer_memnmov(xu_buffer_ref_t buffer, xu_size_t b, xu_size_t n)
{
    return xu_buffer_memnmovp(buffer, 0, b, n);
}
xu_byte_t* xu_buffer_memnmovp(xu_buffer_ref_t buffer, xu_size_t p, xu_size_t b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && (b + n) <= xu_buffer_size(buffer), xu_null);

    // clear?
    if (b == xu_buffer_size(buffer))
    {
        xu_buffer_clear(buffer);
        return xu_buffer_data(buffer);
    }

    // check
    xu_check_return_val(p != b && n, xu_buffer_data(buffer));

    // resize
    xu_byte_t* d = xu_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memmov
    xu_memmov(d + p, d + b, n);
    return d;
}
xu_byte_t* xu_buffer_memcat(xu_buffer_ref_t buffer, xu_buffer_ref_t b)
{
    return xu_buffer_memncat(buffer, xu_buffer_data(b), xu_buffer_size(b));
}
xu_byte_t* xu_buffer_memncat(xu_buffer_ref_t buffer, xu_byte_t const* b, xu_size_t n)
{
    // check
    xu_assert_and_check_return_val(buffer && b, xu_null);

    // check
    xu_check_return_val(n, xu_buffer_data(buffer));

    // is null?
    xu_size_t p = xu_buffer_size(buffer);
    if (!p) return xu_buffer_memncpy(buffer, b, n);

    // resize
    xu_byte_t* d = xu_buffer_resize(buffer, p + n);
    xu_assert_and_check_return_val(d, xu_null);

    // memcat
    xu_memcpy(d + p, b, n);

    // ok?
    return d;
}
