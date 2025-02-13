#include "xutil/platform/file.h"
#include "xutil/platform/syserror.h"
#include "xutil/stream/impl/stream.h"

/* *******************************************************
 * macros
 */

// the file cache maxn
#define XU_STREAM_FILE_CACHE_MAXN XU_FILE_DIRECT_CSIZE

/* *******************************************************
 * types
 */

// the file stream type
typedef struct __xu_stream_file_t
{
    // the file handle
    xu_file_ref_t file;

    // the last read size
    xu_long_t read;

    // the offset
    xu_hize_t offset;

    // the file mode
    xu_size_t mode;

    // is stream file?
    xu_bool_t bstream;

} xu_stream_file_t;

/* *******************************************************
 * implementation
 */

static __xu_inline__ xu_stream_file_t* xu_stream_file_cast(xu_stream_ref_t stream)
{
    // check
    xu_assert_and_check_return_val(stream && xu_stream_type(stream) == XU_STREAM_TYPE_FILE, xu_null);

    // ok?
    return (xu_stream_file_t*)stream;
}
static xu_bool_t xu_stream_file_open(xu_stream_ref_t stream)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && !stream_file->file, xu_false);

    // opened?
    xu_check_return_val(!stream_file->file, xu_true);

    // url
    xu_char_t const* url = xu_url_cstr(xu_stream_url(stream));
    xu_assert_and_check_return_val(url, xu_false);

    // open file
    stream_file->file = xu_file_init(url, stream_file->mode);

    // open file failed?
    if (!stream_file->file)
    {
        // save state
        xu_stream_state_set(stream, xu_syserror_state());
        return xu_false;
    }

    // init offset
    stream_file->offset = 0;

    // ok
    return xu_true;
}
static xu_bool_t xu_stream_file_clos(xu_stream_ref_t stream)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file, xu_false);

    // exit file
    if (stream_file->file && !xu_file_exit(stream_file->file)) return xu_false;
    stream_file->file = xu_null;

    // clear offset
    stream_file->offset = 0;

    // ok
    return xu_true;
}
static xu_long_t xu_stream_file_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && stream_file->file, -1);

    // check
    xu_check_return_val(data, -1);
    xu_check_return_val(size, 0);

    // read
    stream_file->read = xu_file_read(stream_file->file, data, size);
    if (stream_file->read > 0) stream_file->offset += stream_file->read;

    // ok?
    return stream_file->read;
}
static xu_long_t xu_stream_file_writ(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && stream_file->file && data, -1);

    // check
    xu_check_return_val(size, 0);

    // not support for stream file
    xu_assert_and_check_return_val(!stream_file->bstream, -1);

    // writ
    xu_long_t writ = xu_file_writ(stream_file->file, data, size);
    if (writ > 0) stream_file->offset += writ;
    return writ;
}
static xu_bool_t xu_stream_file_sync(xu_stream_ref_t stream, xu_bool_t bclosing)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && stream_file->file, xu_false);

    // not support for stream file
    xu_assert_and_check_return_val(!stream_file->bstream, -1);

    // sync
    return xu_file_sync(stream_file->file);
}
static xu_bool_t xu_stream_file_seek(xu_stream_ref_t stream, xu_hize_t offset)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && stream_file->file, xu_false);

    // is stream file?
    xu_check_return_val(!stream_file->bstream, xu_false);

    // seek
    if (xu_file_seek(stream_file->file, offset, XU_FILE_SEEK_BEG) == offset)
    {
        stream_file->offset = offset;
        return xu_true;
    }
    return xu_false;
}
static xu_long_t xu_stream_file_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file && stream_file->file, -1);

    // wait
    xu_long_t events = 0;
    if (wait & XU_STREAM_WAIT_READ)
    {
        if (stream_file->bstream)
            events |= XU_STREAM_WAIT_READ;
        else
        {
            if (stream_file->offset < xu_file_size(stream_file->file))
                events |= XU_STREAM_WAIT_READ;
            else
                events = -1;
        }
    }
    else if (wait & XU_STREAM_WAIT_WRIT)
        events |= XU_STREAM_WAIT_WRIT;

    // end?
    if (stream_file->bstream && events > 0 && !stream_file->read) events = -1;

    // ok?
    return events;
}
static xu_bool_t xu_stream_file_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args)
{
    // check
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    xu_assert_and_check_return_val(stream_file, xu_false);

    // ctrl
    switch (ctrl)
    {
    case XU_STREAM_CTRL_GET_SIZE:
    {
        // the psize
        xu_hong_t* psize = (xu_hong_t*)xu_va_arg(args, xu_hong_t*);
        xu_assert_and_check_return_val(psize, xu_false);

        // get size
        if (!stream_file->bstream)
        {
            xu_hize_t filesize = stream_file->file ? xu_file_size(stream_file->file) : 0;
            if (!filesize)
            {
                // we cannot use fstat() to get file size in some case (e.g. docker mount fs)
                // so we try to get size from the file path
                xu_file_info_t   info = {0};
                xu_char_t const* url  = xu_url_cstr(xu_stream_url(stream));
                if (url && xu_file_info(url, &info)) filesize = info.size;
            }
            *psize = filesize;
        }
        else
            *psize = -1;
        return xu_true;
    }
    case XU_STREAM_CTRL_FILE_SET_MODE:
    {
        stream_file->mode = (xu_size_t)xu_va_arg(args, xu_size_t);
        return xu_true;
    }
    case XU_STREAM_CTRL_FILE_GET_MODE:
    {
        // the pmode
        xu_size_t* pmode = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pmode, xu_false);

        // get mode
        *pmode = stream_file->mode;
        return xu_true;
    }
    case XU_STREAM_CTRL_FILE_AS_STREAM:
    {
        stream_file->bstream = (xu_bool_t)xu_va_arg(args, xu_bool_t);
        return xu_true;
    }
    case XU_STREAM_CTRL_FILE_GET_FILE:
    {
        // the pfile
        xu_file_ref_t* pfile = (xu_file_ref_t*)xu_va_arg(args, xu_file_ref_t*);
        xu_assert_and_check_return_val(pfile, xu_false);

        // get file
        *pfile = stream_file->file;
        return xu_true;
    }
    default: break;
    }
    return xu_false;
}

/* *******************************************************
 * interfaces
 */
xu_stream_ref_t xu_stream_init_file()
{
    // init stream
    xu_stream_ref_t stream =
        xu_stream_init(XU_STREAM_TYPE_FILE, sizeof(xu_stream_file_t), XU_STREAM_FILE_CACHE_MAXN, xu_stream_file_open,
                       xu_stream_file_clos, xu_null, xu_stream_file_ctrl, xu_stream_file_wait, xu_stream_file_read,
                       xu_stream_file_writ, xu_stream_file_seek, xu_stream_file_sync, xu_null);
    xu_assert_and_check_return_val(stream, xu_null);

    // init the file stream
    xu_stream_file_t* stream_file = xu_stream_file_cast(stream);
    if (stream_file)
    {
        // init it
        stream_file->mode    = XU_FILE_MODE_RO;
        stream_file->bstream = xu_false;
        stream_file->read    = 0;
    }

    // ok?
    return (xu_stream_ref_t)stream;
}
xu_stream_ref_t xu_stream_init_from_file(xu_char_t const* path, xu_size_t mode)
{
    // check
    xu_assert_and_check_return_val(path, xu_null);

    // done
    xu_bool_t       ok     = xu_false;
    xu_stream_ref_t stream = xu_null;
    do
    {
        // init stream
        stream = xu_stream_init_file();
        xu_assert_and_check_break(stream);

        // set path
        if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_SET_URL, path)) break;

        // set mode
        if (mode)
            if (!xu_stream_ctrl(stream, XU_STREAM_CTRL_FILE_SET_MODE, mode)) break;

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
