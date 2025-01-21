#include "xutil/memory/queue_buffer.h"
#include "xutil/libc/string/string.h"
#include "xutil/macros/assert.h"
#include "xutil/macros/malloc.h"

xu_bool_t xu_queue_buffer_init(xu_queue_buffer_ref_t buffer, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_false);

    // init
    buffer->data = xu_null;
    buffer->head = xu_null;
    buffer->size = 0;
    buffer->maxn = maxn;

    // ok
    return xu_true;
}
xu_void_t xu_queue_buffer_exit(xu_queue_buffer_ref_t buffer)
{
    if (buffer)
    {
        if (buffer->data) xu_free(buffer->data);
        xu_memset(buffer, 0, sizeof(xu_queue_buffer_t));
    }
}
xu_byte_t* xu_queue_buffer_data(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // the data
    return buffer->data;
}
xu_byte_t* xu_queue_buffer_head(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // the head
    return buffer->head;
}
xu_byte_t* xu_queue_buffer_tail(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // the tail
    return buffer->head ? buffer->head + buffer->size : xu_null;
}
xu_size_t xu_queue_buffer_size(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the size
    return buffer->size;
}
xu_size_t xu_queue_buffer_maxn(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, 0);

    // the maxn
    return buffer->maxn;
}
xu_size_t xu_queue_buffer_left(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer && buffer->size <= buffer->maxn, 0);

    // the left
    return buffer->maxn - buffer->size;
}
xu_bool_t xu_queue_buffer_full(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_false);

    // is full?
    return buffer->size == buffer->maxn ? xu_true : xu_false;
}
xu_bool_t xu_queue_buffer_null(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_false);

    // is null?
    return buffer->size ? xu_false : xu_true;
}
xu_void_t xu_queue_buffer_clear(xu_queue_buffer_ref_t buffer)
{
    // check
    xu_assert_and_check_return(buffer);

    // clear it
    buffer->size = 0;
    buffer->head = buffer->data;
}
xu_byte_t* xu_queue_buffer_resize(xu_queue_buffer_ref_t buffer, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(buffer && maxn && maxn >= buffer->size, xu_null);

    // has data?
    if (buffer->data)
    {
        // move data to head
        if (buffer->head != buffer->data)
        {
            if (buffer->size) xu_memmov(buffer->data, buffer->head, buffer->size);
            buffer->head = buffer->data;
        }

        // realloc
        if (maxn > buffer->maxn)
        {
            // init head
            buffer->head = xu_null;

            // make data
            buffer->data = (xu_byte_t*)xu_ralloc(buffer->data, maxn);
            xu_assert_and_check_return_val(buffer->data, xu_null);

            // save head
            buffer->head = buffer->data;
        }
    }

    // update maxn
    buffer->maxn = maxn;

    // ok
    return buffer->data;
}
xu_long_t xu_queue_buffer_skip(xu_queue_buffer_ref_t buffer, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(buffer, -1);

    // no data?
    xu_check_return_val(buffer->data && buffer->size && size, 0);
    xu_assert_and_check_return_val(buffer->head, -1);

    // read data
    xu_long_t read = buffer->size > size ? size : buffer->size;
    buffer->head += read;
    buffer->size -= read;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;

    // ok
    return read;
}
xu_long_t xu_queue_buffer_read(xu_queue_buffer_ref_t buffer, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(buffer && data, -1);

    // no data?
    xu_check_return_val(buffer->data && buffer->size && size, 0);
    xu_assert_and_check_return_val(buffer->head, -1);

    // read data
    xu_long_t read = buffer->size > size ? size : buffer->size;
    xu_memcpy(data, buffer->head, read);
    buffer->head += read;
    buffer->size -= read;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;

    // ok
    return read;
}
xu_long_t xu_queue_buffer_writ(xu_queue_buffer_ref_t buffer, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(buffer && data && buffer->maxn, -1);

    // no data?
    if (!buffer->data)
    {
        // make data
        buffer->data = xu_malloc_bytes(buffer->maxn);
        xu_assert_and_check_return_val(buffer->data, -1);

        // init it
        buffer->head = buffer->data;
        buffer->size = 0;
    }
    xu_assert_and_check_return_val(buffer->data && buffer->head, -1);

    // full?
    xu_size_t left = buffer->maxn - buffer->size;
    xu_check_return_val(left, 0);

    // attempt to write data in tail directly if the tail space is enough
    xu_byte_t* tail = buffer->head + buffer->size;
    if (buffer->data + buffer->maxn >= tail + size)
    {
        xu_memcpy(tail, data, size);
        buffer->size += size;
        return (xu_long_t)size;
    }

    // move data to head
    if (buffer->head != buffer->data)
    {
        if (buffer->size) xu_memmov(buffer->data, buffer->head, buffer->size);
        buffer->head = buffer->data;
    }

    // write data
    xu_size_t writ = left > size ? size : left;
    xu_memcpy(buffer->data + buffer->size, data, writ);
    buffer->size += writ;

    // ok
    return writ;
}
xu_byte_t* xu_queue_buffer_pull_init(xu_queue_buffer_ref_t buffer, xu_size_t* size)
{
    // check
    xu_assert_and_check_return_val(buffer, xu_null);

    // no data?
    xu_check_return_val(buffer->data && buffer->size, xu_null);
    xu_assert_and_check_return_val(buffer->head, xu_null);

    // save size
    if (size) *size = buffer->size;

    // ok
    return buffer->head;
}
xu_void_t xu_queue_buffer_pull_exit(xu_queue_buffer_ref_t buffer, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buffer && buffer->head && size <= buffer->size);

    // update
    buffer->size -= size;
    buffer->head += size;

    // null? reset head
    if (!buffer->size) buffer->head = buffer->data;
}
xu_byte_t* xu_queue_buffer_push_init(xu_queue_buffer_ref_t buffer, xu_size_t* size)
{
    // check
    xu_assert_and_check_return_val(buffer && buffer->maxn, xu_null);

    // no data?
    if (!buffer->data)
    {
        // make data
        buffer->data = xu_malloc_bytes(buffer->maxn);
        xu_assert_and_check_return_val(buffer->data, xu_null);

        // init
        buffer->head = buffer->data;
        buffer->size = 0;
    }
    xu_assert_and_check_return_val(buffer->data && buffer->head, xu_null);

    // full?
    xu_size_t left = buffer->maxn - buffer->size;
    xu_check_return_val(left, xu_null);

    // move data to head first, make sure there is enough write space
    if (buffer->head != buffer->data)
    {
        if (buffer->size) xu_memmov(buffer->data, buffer->head, buffer->size);
        buffer->head = buffer->data;
    }

    // save size
    if (size) *size = left;

    // ok
    return buffer->head + buffer->size;
}
xu_void_t xu_queue_buffer_push_exit(xu_queue_buffer_ref_t buffer, xu_size_t size)
{
    // check
    xu_assert_and_check_return(buffer && buffer->head && buffer->size + size <= buffer->maxn);

    // update the size
    buffer->size += size;
}
