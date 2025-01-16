#include "xutil/stream/stream.h"
#include "xutil/utils/base64.h"

// the data stream type
typedef struct __xu_stream_data_t
{
    // the data
    xu_byte_t* data;

    // the head
    xu_byte_t* head;

    // the size
    xu_size_t size;

    // the data is referenced?
    xu_bool_t bref;

} xu_stream_data_t;

static __xu_inline__ xu_stream_data_t* xu_stream_data_cast(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && xu_stream_type(stream) == XU_STREAM_TYPE_DATA, xu_null);

    // ok?
    return (xu_stream_data_t*)stream;
}
static xu_bool_t xu_stream_data_open(xu_stream_ref_t stream)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data && stream_data->data && stream_data->size, xu_false);

    // init head
    stream_data->head = stream_data->data;

    // ok
    return xu_true;
}
static xu_bool_t xu_stream_data_clos(xu_stream_ref_t stream)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data, xu_false);

    // clear head
    stream_data->head = xu_null;

    // ok
    return xu_true;
}
static xu_void_t xu_stream_data_exit(xu_stream_ref_t stream)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return(stream_data);

    // clear head
    stream_data->head = xu_null;

    // exit data
    if (stream_data->data && !stream_data->bref) xu_free(stream_data->data);
    stream_data->data = xu_null;
    stream_data->size = 0;
}
static xu_long_t xu_stream_data_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data && stream_data->data && stream_data->head, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // the left
    xu_size_t left = stream_data->data + stream_data->size - stream_data->head;

    // the need
    if (size > left) size = left;

    // read data
    if (size) xu_memcpy(data, stream_data->head, size);

    // save head
    stream_data->head += size;

    // ok?
    return (xu_long_t)(size);
}
static xu_long_t xu_stream_data_writ(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data && stream_data->data && stream_data->head, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // the left
    xu_size_t left = stream_data->data + stream_data->size - stream_data->head;

    // the need
    if (size > left) size = left;

    // writ data
    if (size) xu_memcpy(stream_data->head, data, size);

    // save head
    stream_data->head += size;

    // ok?
    return left ? (xu_long_t)(size) : -1; // force end if full
}
static xu_bool_t xu_stream_data_seek(xu_stream_ref_t stream, xu_hize_t offset)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data && offset <= stream_data->size, xu_false);

    // seek
    stream_data->head = stream_data->data + offset;

    // ok
    return xu_true;
}
static xu_long_t xu_stream_data_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data, -1);

    // wait
    xu_long_t events = 0;
    if (stream_data->head < stream_data->data + stream_data->size)
    {
        if (wait & XU_STREAM_WAIT_READ) events |= XU_STREAM_WAIT_READ;
        if (wait & XU_STREAM_WAIT_WRIT) events |= XU_STREAM_WAIT_WRIT;
    }
    else
        events = -1;

    // ok?
    return events;
}
static xu_bool_t xu_stream_data_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_stream_data_t* stream_data = xu_stream_data_cast(stream);
    xu_assert_and_check_return_val(stream_data, xu_false);

    // ctrl
    switch (ctrl)
    {
    case XU_STREAM_CTRL_GET_SIZE:
    {
        // the psize
        xu_hong_t* psize = (xu_hong_t*)xu_va_arg(args, xu_hong_t*);
        xu_assert_and_check_return_val(psize, xu_false);

        // get size
        *psize = stream_data->size;
        return xu_true;
    }
    case XU_STREAM_CTRL_DATA_SET_DATA:
    {
        // exit data first if exists
        if (stream_data->data && !stream_data->bref) xu_free(stream_data->data);

        // save data
        stream_data->data = (xu_byte_t*)xu_va_arg(args, xu_byte_t*);
        stream_data->size = (xu_size_t)xu_va_arg(args, xu_size_t);
        stream_data->head = xu_null;
        stream_data->bref = xu_true;

        // check
        xu_assert_and_check_return_val(stream_data->data && stream_data->size, xu_false);
        return xu_true;
    }
    case XU_STREAM_CTRL_SET_URL:
    {
        // check
        xu_assert_and_check_return_val(xu_stream_is_closed((xu_stream_ref_t)stream), xu_false);

        // set url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(url, xu_false);

        // the url size
        xu_size_t url_size = xu_strlen(url);
        xu_assert_and_check_return_val(url_size > 7, xu_false);

        // the base64 data and size
        xu_char_t const* base64_data = url + 7;
        xu_size_t        base64_size = url_size - 7;

        // make data
        xu_size_t  maxn = base64_size;
        xu_byte_t* data = xu_malloc_bytes(maxn);
        xu_assert_and_check_return_val(data, xu_false);

        // decode base64 data
        xu_size_t size = xu_base64_decode(base64_data, base64_size, data, maxn);
        xu_assert_and_check_return_val(size, xu_false);

        // exit data first if exists
        if (stream_data->data && !stream_data->bref) xu_free(stream_data->data);

        // save data
        stream_data->data = data;
        stream_data->size = size;
        stream_data->bref = xu_false;
        stream_data->head = xu_null;

        // ok
        return xu_true;
    }
    break;
    default: break;
    }
    return xu_false;
}

xu_stream_ref_t xu_stream_init_data()
{
    return xu_stream_init(XU_STREAM_TYPE_DATA, sizeof(xu_stream_data_t), 0, xu_stream_data_open, xu_stream_data_clos,
                          xu_stream_data_exit, xu_stream_data_ctrl, xu_stream_data_wait, xu_stream_data_read,
                          xu_stream_data_writ, xu_stream_data_seek, xu_null, xu_null);
}
xu_stream_ref_t xu_stream_init_from_data(xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return_val(data && size, xu_null);

    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init_data();
        xu_assert_and_check_break(stream);

        // set data and size
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_DATA_SET_DATA, data, size)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) xu_stream_exit(stream);
        stream = xu_null;
    }

    // ok
    return stream;
}
