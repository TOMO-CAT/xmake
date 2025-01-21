#pragma once

#include "xutil/network/url.h"
#include "xutil/prefix.h"
#include "xutil/stream/prefix.h"

/*!architecture
 *
 *
 * <pre>
 *
 *                                - data
 *                               |
 *  stream ---------------------  - file
 *     |                         |
 *     |                          - sock
 *     |                         |
 *     |                          - http
 *     |           - charset
 *     |          |
 *     - filter - |- chunked
 *                |
 *                |- cache
 *                |
 *                 - zip
 *
 *
 * url:
 * data://base64
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * socks://host:port
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 *
 */

/* *******************************************************
 *  macros
 * *******************************************************
 */

// the stream block maxn
#define XU_STREAM_BLOCK_MAXN (8192)

// the stream bitops
#ifdef XU_WORDS_BIGENDIAN
#    define xu_stream_bread_u16_ne(stream, pvalue) xu_stream_bread_u16_be(stream, pvalue)
#    define xu_stream_bread_s16_ne(stream, pvalue) xu_stream_bread_s16_be(stream, pvalue)
#    define xu_stream_bread_u24_ne(stream, pvalue) xu_stream_bread_u24_be(stream, pvalue)
#    define xu_stream_bread_s24_ne(stream, pvalue) xu_stream_bread_s24_be(stream, pvalue)
#    define xu_stream_bread_u32_ne(stream, pvalue) xu_stream_bread_u32_be(stream, pvalue)
#    define xu_stream_bread_s32_ne(stream, pvalue) xu_stream_bread_s32_be(stream, pvalue)
#    define xu_stream_bread_u64_ne(stream, pvalue) xu_stream_bread_u64_be(stream, pvalue)
#    define xu_stream_bread_s64_ne(stream, pvalue) xu_stream_bread_s64_be(stream, pvalue)

#    define xu_stream_bwrit_u16_ne(stream, value) xu_stream_bwrit_u16_be(stream, value)
#    define xu_stream_bwrit_s16_ne(stream, value) xu_stream_bwrit_s16_be(stream, value)
#    define xu_stream_bwrit_u24_ne(stream, value) xu_stream_bwrit_u24_be(stream, value)
#    define xu_stream_bwrit_s24_ne(stream, value) xu_stream_bwrit_s24_be(stream, value)
#    define xu_stream_bwrit_u32_ne(stream, value) xu_stream_bwrit_u32_be(stream, value)
#    define xu_stream_bwrit_s32_ne(stream, value) xu_stream_bwrit_s32_be(stream, value)
#    define xu_stream_bwrit_u64_ne(stream, value) xu_stream_bwrit_u64_be(stream, value)
#    define xu_stream_bwrit_s64_ne(stream, value) xu_stream_bwrit_s64_be(stream, value)

#else
#    define xu_stream_bread_u16_ne(stream, pvalue) xu_stream_bread_u16_le(stream, pvalue)
#    define xu_stream_bread_s16_ne(stream, pvalue) xu_stream_bread_s16_le(stream, pvalue)
#    define xu_stream_bread_u24_ne(stream, pvalue) xu_stream_bread_u24_le(stream, pvalue)
#    define xu_stream_bread_s24_ne(stream, pvalue) xu_stream_bread_s24_le(stream, pvalue)
#    define xu_stream_bread_u32_ne(stream, pvalue) xu_stream_bread_u32_le(stream, pvalue)
#    define xu_stream_bread_s32_ne(stream, pvalue) xu_stream_bread_s32_le(stream, pvalue)
#    define xu_stream_bread_u64_ne(stream, pvalue) xu_stream_bread_u64_le(stream, pvalue)
#    define xu_stream_bread_s64_ne(stream, pvalue) xu_stream_bread_s64_le(stream, pvalue)

#    define xu_stream_bwrit_u16_ne(stream, value) xu_stream_bwrit_u16_le(stream, value)
#    define xu_stream_bwrit_s16_ne(stream, value) xu_stream_bwrit_s16_le(stream, value)
#    define xu_stream_bwrit_u24_ne(stream, value) xu_stream_bwrit_u24_le(stream, value)
#    define xu_stream_bwrit_s24_ne(stream, value) xu_stream_bwrit_s24_le(stream, value)
#    define xu_stream_bwrit_u32_ne(stream, value) xu_stream_bwrit_u32_le(stream, value)
#    define xu_stream_bwrit_s32_ne(stream, value) xu_stream_bwrit_s32_le(stream, value)
#    define xu_stream_bwrit_u64_ne(stream, value) xu_stream_bwrit_u64_le(stream, value)
#    define xu_stream_bwrit_s64_ne(stream, value) xu_stream_bwrit_s64_le(stream, value)

#endif

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT
#    ifdef XU_FLOAT_BIGENDIAN
#        define xu_stream_bread_double_nbe(stream, pvalue) xu_stream_bread_double_bbe(stream, pvalue)
#        define xu_stream_bread_double_nle(stream, pvalue) xu_stream_bread_double_ble(stream, pvalue)

#        define xu_stream_bwrit_double_nbe(stream, value) xu_stream_bwrit_double_bbe(stream, value)
#        define xu_stream_bwrit_double_nle(stream, value) xu_stream_bwrit_double_ble(stream, value)
#    else
#        define xu_stream_bread_double_nbe(stream, pvalue) xu_stream_bread_double_lbe(stream, pvalue)
#        define xu_stream_bread_double_nle(stream, pvalue) xu_stream_bread_double_lle(stream, pvalue)

#        define xu_stream_bwrit_double_nbe(stream, value) xu_stream_bwrit_double_lbe(stream, value)
#        define xu_stream_bwrit_double_nle(stream, value) xu_stream_bwrit_double_lle(stream, value)
#    endif
#    ifdef XU_WORDS_BIGENDIAN
#        define xu_stream_bread_float_ne(stream, pvalue) xu_stream_bread_float_be(stream, pvalue)
#        define xu_stream_bwrit_float_ne(stream, value) xu_stream_bwrit_float_be(stream, value)

#        define xu_stream_bread_double_nne(stream, pvalue) xu_stream_bread_double_nbe(stream, pvalue)
#        define xu_stream_bread_double_bne(stream, pvalue) xu_stream_bread_double_bbe(stream, pvalue)
#        define xu_stream_bread_double_lne(stream, pvalue) xu_stream_bread_double_lbe(stream, pvalue)

#        define xu_stream_bwrit_double_nne(stream, value) xu_stream_bwrit_double_nbe(stream, value)
#        define xu_stream_bwrit_double_bne(stream, value) xu_stream_bwrit_double_bbe(stream, value)
#        define xu_stream_bwrit_double_lne(stream, value) xu_stream_bwrit_double_lbe(stream, value)
#    else
#        define xu_stream_bread_float_ne(stream, pvalue) xu_stream_bread_float_le(stream, pvalue)
#        define xu_stream_bwrit_float_ne(stream, value) xu_stream_bwrit_float_le(stream, value)

#        define xu_stream_bread_double_nne(stream, pvalue) xu_stream_bread_double_nle(stream, pvalue)
#        define xu_stream_bread_double_bne(stream, pvalue) xu_stream_bread_double_ble(stream, pvalue)
#        define xu_stream_bread_double_lne(stream, pvalue) xu_stream_bread_double_lle(stream, pvalue)

#        define xu_stream_bwrit_double_nne(stream, value) xu_stream_bwrit_double_nle(stream, value)
#        define xu_stream_bwrit_double_bne(stream, value) xu_stream_bwrit_double_ble(stream, value)
#        define xu_stream_bwrit_double_lne(stream, value) xu_stream_bwrit_double_lle(stream, value)
#    endif
#endif

/* *******************************************************
 *  interfaces
 * *******************************************************
 */

/*! init stream
 *
 * @param type          the stream type
 * @param type_size     the stream type size
 * @param cache         the cache size
 * @param open          the stream impl func: open
 * @param clos          the stream impl func: clos
 * @param exit          the stream impl func: exit, optional
 * @param ctrl          the stream impl func: ctrl
 * @param wait          the stream impl func: wait
 * @param read          the stream impl func: read
 * @param writ          the stream impl func: writ
 * @param seek          the stream impl func: seek, optional
 * @param sync          the stream impl func: sync, optional
 * @param kill          the stream impl func: kill, optional
 *
 * @return              the stream
 *
 * @code
    // the custom xxxx stream type
    typedef struct __xu_stream_xxxx_impl_t
    {
        // the xxxx data
        xu_handle_t         xxxx;

    }xu_stream_xxxx_impl_t;

    static xu_bool_t xu_stream_xxxx_impl_open(xu_stream_ref_t stream)
    {
        // check
        xu_stream_xxxx_impl_t* impl = (xu_stream_xxxx_impl_t*)stream;
        xu_assert_and_check_return_val(impl, xu_false);

        // ok
        return xu_true;
    }
    static xu_bool_t xu_stream_xxxx_impl_clos(xu_stream_ref_t stream)
    {
        // check
        xu_stream_xxxx_impl_t* impl = (xu_stream_xxxx_impl_t*)stream;
        xu_assert_and_check_return_val(impl, xu_false);

        // ok
        return xu_true;
    }

    // define other xxxx stream func
    // ...

    // init stream
    xu_stream_ref_t stream = xu_stream_init(    XU_STREAM_TYPE_XXXX
                                            ,   sizeof(xu_stream_xxxx_impl_t)
                                            ,   0
                                            ,   xu_stream_xxxx_impl_open
                                            ,   xu_stream_xxxx_impl_clos
                                            ,   xu_stream_xxxx_impl_exit
                                            ,   xu_stream_xxxx_impl_ctrl
                                            ,   xu_stream_xxxx_impl_wait
                                            ,   xu_stream_xxxx_impl_read
                                            ,   xu_stream_xxxx_impl_writ
                                            ,   xu_stream_xxxx_impl_seek
                                            ,   xu_stream_xxxx_impl_sync
                                            ,   xu_stream_xxxx_impl_kill);

    // using stream
    // ...

 * @endcode
 */
xu_stream_ref_t xu_stream_init(xu_size_t type, xu_size_t type_size, xu_size_t cache,
                               xu_bool_t (*open)(xu_stream_ref_t stream), xu_bool_t (*clos)(xu_stream_ref_t stream),
                               xu_void_t (*exit)(xu_stream_ref_t stream),
                               xu_bool_t (*ctrl)(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args),
                               xu_long_t (*wait)(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout),
                               xu_long_t (*read)(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size),
                               xu_long_t (*writ)(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size),
                               xu_bool_t (*seek)(xu_stream_ref_t stream, xu_hize_t offset),
                               xu_bool_t (*sync)(xu_stream_ref_t stream, xu_bool_t bclosing),
                               xu_void_t (*kill)(xu_stream_ref_t stream));

/*! init data stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_data(xu_noarg_t);

/*! init file stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_file(xu_noarg_t);

/*! init sock stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_sock(xu_noarg_t);

/*! init http stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_http(xu_noarg_t);

/*! init filter stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter(xu_noarg_t);

/*! exit stream
 *
 * @param stream        the stream
 */
xu_void_t xu_stream_exit(xu_stream_ref_t stream);

/*! init stream from url
 *
 * @code
 *
    // init stream
    xu_stream_ref_t stream = xu_stream_init_from_url("http://www.xxx.com/file");
    if (stream)
    {
        // open stream
        if (xu_stream_open(stream))
        {
            // ...
        }

        // exit stream
        xu_stream_exit(stream);
    }
 *
 * @endcode
 *
 * @param url           the url
 *                      - data://base64
 *                      - file://path or unix path: e.g. /root/xxxx/file
 *                      - sock://host:port?tcp=
 *                      - sock://host:port?udp=
 *                      - socks://host:port
 *                      - http://host:port/path?arg0=&arg1=...
 *                      - https://host:port/path?arg0=&arg1=...
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_url(xu_char_t const* url);

/*! init stream from data
 *
 * @param data          the data
 * @param size          the size
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_data(xu_byte_t const* data, xu_size_t size);

/*! init stream from file
 *
 * @param path          the file path
 * @param mode          the file mode, using the default ro mode if zero
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_file(xu_char_t const* path, xu_size_t mode);

/*! init stream from sock
 *
 * @param host          the host
 * @param port          the port
 * @param type          the socket type, tcp or udp
 * @param bssl          enable ssl?
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_sock(xu_char_t const* host, xu_uint16_t port, xu_size_t type, xu_bool_t bssl);

/*! init stream from sock reference
 *
 * @param sock          the socket
 * @param type          the socket type, tcp or udp
 * @param bssl          enable ssl?
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_sock_ref(xu_socket_ref_t sock, xu_size_t type, xu_bool_t bssl);

/*! init stream from http or https
 *
 * @param host          the host
 * @param port          the port
 * @param path          the path
 * @param bssl          enable ssl?
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_from_http(xu_char_t const* host, xu_uint16_t port, xu_char_t const* path,
                                         xu_bool_t bssl);

/*! init filter stream from null
 *
 * @param stream        the stream
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter_from_null(xu_stream_ref_t stream);

/*! init filter stream from zip
 *
 * @param stream        the stream
 * @param algo          the zip algorithm
 * @param action        the zip action
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter_from_zip(xu_stream_ref_t stream, xu_size_t algo, xu_size_t action);

/*! init filter stream from cache
 *
 * @param stream        the stream
 * @param size          the initial cache size, using the default size if be zero
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter_from_cache(xu_stream_ref_t stream, xu_size_t size);

/*! init filter stream from charset
 *
 * @param stream        the stream
 * @param fr            the from charset
 * @param to            the to charset
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter_from_charset(xu_stream_ref_t stream, xu_size_t fr, xu_size_t to);

/*! init filter stream from chunked
 *
 * @param stream        the stream
 * @param dechunked     decode the chunked data?
 *
 * @return              the stream
 */
xu_stream_ref_t xu_stream_init_filter_from_chunked(xu_stream_ref_t stream, xu_bool_t dechunked);

/*! wait stream
 *
 * blocking wait the single event object, so need not aiop
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param stream        the stream
 * @param wait          the wait type
 * @param timeout       the timeout value, return immediately if 0, infinity if -1
 *
 * @return              the event type, return 0 if timeout, return -1 if error
 */
xu_long_t xu_stream_wait(xu_stream_ref_t stream, xu_size_t wait, xu_long_t timeout);

/*! the state
 *
 * @param stream        the stream
 *
 * @return              the state
 */
xu_size_t xu_stream_state(xu_stream_ref_t stream);

/*! set the state
 *
 * @param stream        the stream
 * @param state         the state
 */
xu_void_t xu_stream_state_set(xu_stream_ref_t stream, xu_size_t state);

/*! the stream type
 *
 * @param stream        the stream
 *
 * @return              the stream type
 */
xu_size_t xu_stream_type(xu_stream_ref_t stream);

/*! the stream size and not seeking it
 *
 * @param stream        the stream
 *
 * @return              the stream size, no size: -1, empty or error: 0
 */
xu_hong_t xu_stream_size(xu_stream_ref_t stream);

/*! the stream left size and not seeking it
 *
 * @param stream        the stream
 *
 * @return              the stream left size, no size: infinity, empty or end: 0
 */
xu_hize_t xu_stream_left(xu_stream_ref_t stream);

/*! the stream is end?
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_beof(xu_stream_ref_t stream);

/*! the stream offset
 *
 * the offset is read + writ and using seek for modifying it if size != -1, .e.g: data, file, ..
 * the offset is calculated from the last read/writ and not seeking it if size == -1, .e.g: sock, filter, ..
 *
 * @param stream        the stream
 *
 * @return              the stream offset
 */
xu_hize_t xu_stream_offset(xu_stream_ref_t stream);

/*! is opened?
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_is_opened(xu_stream_ref_t stream);

/*! is closed?
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_is_closed(xu_stream_ref_t stream);

/*! is killed?
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_is_killed(xu_stream_ref_t stream);

/*! the stream url
 *
 * @param stream        the stream
 *
 * @return              the stream url
 */
xu_url_ref_t xu_stream_url(xu_stream_ref_t stream);

/*! the stream timeout
 *
 * @param stream        the stream
 *
 * @return              the stream timeout
 */
xu_long_t xu_stream_timeout(xu_stream_ref_t stream);

/*! ctrl stream
 *
 * @param stream        the stream
 * @param ctrl          the ctrl code
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_ctrl(xu_stream_ref_t stream, xu_size_t ctrl, ...);

/*! ctrl stream with arguments
 *
 * @param stream        the stream
 * @param ctrl          the ctrl code
 * @param args          the ctrl args
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_ctrl_with_args(xu_stream_ref_t stream, xu_size_t ctrl, xu_va_list_t args);

/*! kill stream
 *
 * @param stream        the stream
 */
xu_void_t xu_stream_kill(xu_stream_ref_t stream);

/*! open stream
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_open(xu_stream_ref_t stream);

/*! close stream
 *
 * @param stream        the stream
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_clos(xu_stream_ref_t stream);

/*! read data, non-blocking
 *
 * @code

    xu_long_t read = 0;
    xu_byte_t data[XU_STREAM_BLOCK_MAXN];
    while (!xu_stream_beof(stream))
    {
        // read data
        xu_long_t real = xu_stream_read(stream, data, sizeof(data));

        // ok?
        if (real > 0) read += real;
        // no data? continue it
        else if (!real)
        {
            // wait
            real = xu_stream_wait(stream, XU_STREAM_WAIT_READ, xu_stream_timeout(stream));
            xu_check_break(real > 0);

            // has read?
            xu_assert_and_check_break(real & XU_STREAM_WAIT_READ);
        }
        // failed or end?
        else break;
    }

 * @endcode
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_stream_read(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size);

/*! writ data, non-blocking
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
xu_long_t xu_stream_writ(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size);

/*! block read
 *
 * @code
 *
    // get stream size
    //
    // @note
    // size may be < -1 for the http(chunked)/filter/.. stream
    // we need call xu_stream_read for reading data if size < 0
    //
    xu_hong_t size = xu_stream_size(stream);
    xu_assert(size > 0);

    // make data
    xu_byte_t* data = xu_malloc((xu_size_t)size);
    if (data)
    {
        // read data
        xu_bool_t ok = xu_stream_bread(stream, data, size);

        // exit data
        xu_free(data)
    }
 *
 * @endcode
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread(xu_stream_ref_t stream, xu_byte_t* data, xu_size_t size);

/*! block writ
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit(xu_stream_ref_t stream, xu_byte_t const* data, xu_size_t size);

/*! sync stream
 *
 * @param stream        the stream
 * @param bclosing      is closing?
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_sync(xu_stream_ref_t stream, xu_bool_t bclosing);

/*! need stream (block mode)
 *
 * @note it will not update offset, if you want to use block mode, please use xu_stream_peek()
 *
 * @code

    // need 16-bytes data
    xu_byte_t* data = xu_null;
    if (xu_stream_need(stream, &data, 16))
    {
        // ..
    }

 * @endcode
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_need(xu_stream_ref_t stream, xu_byte_t** data, xu_size_t size);

/*! try to peek stream (non-block mode)
 *
 * @note it will not update offset, if you want to use block mode, please use xu_stream_need()
 *
 * @code

    xu_long_t  read = 0;
    xu_byte_t* data = xu_null;
    while (!xu_stream_beof(stream))
    {
        // peek data
        xu_long_t real = xu_stream_peek(stream, &data, XU_STREAM_BLOCK_MAXN);

        // ok?
        if (real > 0)
        {
            // TODO data
            // ...

            read += real;
            if (!xu_stream_skip(stream, real)) break;
        }
        // no data? continue it
        else if (!real)
        {
            // wait
            real = xu_stream_wait(stream, XU_STREAM_WAIT_READ, xu_stream_timeout(stream));
            xu_check_break(real > 0);

            // has read?
            xu_assert_and_check_break(real & XU_STREAM_WAIT_READ);
        }
        // failed or end?
        else break;
    }

 * @endcode
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              the real peek size or -1 (eof)
 */
xu_long_t xu_stream_peek(xu_stream_ref_t stream, xu_byte_t** data, xu_size_t size);

/*! seek stream
 *
 * @param stream        the stream
 * @param offset        the offset
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_seek(xu_stream_ref_t stream, xu_hize_t offset);

/*! skip stream
 *
 * @param stream        the stream
 * @param size          the size
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_skip(xu_stream_ref_t stream, xu_hize_t size);

/*! block writ format data
 *
 * @param stream        the stream
 * @param fmt           the format
 *
 * @return              the real size, failed: -1
 */
xu_long_t xu_stream_printf(xu_stream_ref_t stream, xu_char_t const* fmt, ...);

/*! block read line
 *
 * @code
 *
    // read line
    xu_long_t size = 0;
    xu_char_t line[8192];
    while ((size = xu_stream_bread_line(stream, line, sizeof(line))) >= 0)
    {
        // trace
        xu_trace_i("line: %s", line);
    }
 *
 * @endcode
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size
 */
xu_long_t xu_stream_bread_line(xu_stream_ref_t stream, xu_char_t* data, xu_size_t size);

/*! block writ line
 *
 * @param stream        the stream
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size
 */
xu_long_t xu_stream_bwrit_line(xu_stream_ref_t stream, xu_char_t* data, xu_size_t size);

/*! block read all data
 *
 * @code
 *
    // read all data
    xu_size_t    size = 0;
    xu_byte_t*   data = xu_stream_bread_all(stream, xu_false, &size);
    if (data)
    {
        // exit data
        xu_free(data);
    }

    // read all cstr and append '\0'
    xu_size_t    size = 0;
    xu_char_t*   cstr = (xu_char_t*)xu_stream_bread_all(stream, xu_true, &size);
    if (cstr)
    {
        // exit cstr
        xu_free(cstr);
    }

 * @endcode
 *
 * @param stream        the stream
 * @param is_cstr       will append '\0' if be c-string
 * @param psize         the size pointer, optional
 *
 * @return              the data
 */
xu_byte_t* xu_stream_bread_all(xu_stream_ref_t stream, xu_bool_t is_cstr, xu_size_t* psize);

/*! block read uint8 integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u8(xu_stream_ref_t stream, xu_uint8_t* pvalue);

/*! block read sint8 integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s8(xu_stream_ref_t stream, xu_sint8_t* pvalue);

/*! block read uint16-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u16_le(xu_stream_ref_t stream, xu_uint16_t* pvalue);

/*! block read sint16-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s16_le(xu_stream_ref_t stream, xu_sint16_t* pvalue);

/*! block read uint24-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u24_le(xu_stream_ref_t stream, xu_uint32_t* pvalue);

/*! block read sint24-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s24_le(xu_stream_ref_t stream, xu_sint32_t* pvalue);

/*! block read uint32-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u32_le(xu_stream_ref_t stream, xu_uint32_t* pvalue);

/*! block read sint32-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s32_le(xu_stream_ref_t stream, xu_sint32_t* pvalue);

/*! block read uint64-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u64_le(xu_stream_ref_t stream, xu_uint64_t* pvalue);

/*! block read sint64-le integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s64_le(xu_stream_ref_t stream, xu_sint64_t* pvalue);

/*! block read uint16-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u16_be(xu_stream_ref_t stream, xu_uint16_t* pvalue);

/*! block read sint16-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s16_be(xu_stream_ref_t stream, xu_sint16_t* pvalue);

/*! block read uint24-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u24_be(xu_stream_ref_t stream, xu_uint32_t* pvalue);

/*! block read sint24-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s24_be(xu_stream_ref_t stream, xu_sint32_t* pvalue);

/*! block read uint32-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u32_be(xu_stream_ref_t stream, xu_uint32_t* pvalue);

/*! block read sint32-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s32_be(xu_stream_ref_t stream, xu_sint32_t* pvalue);

/*! block read uint64-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_u64_be(xu_stream_ref_t stream, xu_uint64_t* pvalue);

/*! block read sint64-be integer
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_s64_be(xu_stream_ref_t stream, xu_sint64_t* pvalue);

/*! block writ uint8 integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u8(xu_stream_ref_t stream, xu_uint8_t value);

/*! block writ sint8 integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s8(xu_stream_ref_t stream, xu_sint8_t value);

/*! block writ uint16-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u16_le(xu_stream_ref_t stream, xu_uint16_t value);

/*! block writ sint16-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s16_le(xu_stream_ref_t stream, xu_sint16_t value);

/*! block writ uint24-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u24_le(xu_stream_ref_t stream, xu_uint32_t value);

/*! block writ sint24-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s24_le(xu_stream_ref_t stream, xu_sint32_t value);

/*! block writ uint32-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u32_le(xu_stream_ref_t stream, xu_uint32_t value);

/*! block writ sint32-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s32_le(xu_stream_ref_t stream, xu_sint32_t value);

/*! block writ uint64-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u64_le(xu_stream_ref_t stream, xu_uint64_t value);

/*! block writ sint64-le integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s64_le(xu_stream_ref_t stream, xu_sint64_t value);

/*! block writ uint16-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u16_be(xu_stream_ref_t stream, xu_uint16_t value);

/*! block writ sint16-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s16_be(xu_stream_ref_t stream, xu_sint16_t value);

/*! block writ uint24-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u24_be(xu_stream_ref_t stream, xu_uint32_t value);

/*! block writ sint24-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s24_be(xu_stream_ref_t stream, xu_sint32_t value);

/*! block writ uint32-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u32_be(xu_stream_ref_t stream, xu_uint32_t value);

/*! block writ sint32-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s32_be(xu_stream_ref_t stream, xu_sint32_t value);

/*! block writ uint64-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_u64_be(xu_stream_ref_t stream, xu_uint64_t value);

/*! block writ sint64-be integer
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_s64_be(xu_stream_ref_t stream, xu_sint64_t value);

#ifdef XU_CONFIG_TYPE_HAVE_FLOAT

/*! read float-le number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_float_le(xu_stream_ref_t stream, xu_float_t* pvalue);

/*! read float-be number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_float_be(xu_stream_ref_t stream, xu_float_t* pvalue);

/*! read double-ble number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_double_ble(xu_stream_ref_t stream, xu_double_t* pvalue);

/*! read double-bbe number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_double_bbe(xu_stream_ref_t stream, xu_double_t* pvalue);

/*! read double-lle number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_double_lle(xu_stream_ref_t stream, xu_double_t* pvalue);

/*! read double-lbe number
 *
 * @param stream        the stream
 * @param pvalue        the value pointer
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bread_double_lbe(xu_stream_ref_t stream, xu_double_t* pvalue);

/*! writ float-le number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_float_le(xu_stream_ref_t stream, xu_float_t value);

/*! writ float-be number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_float_be(xu_stream_ref_t stream, xu_float_t value);

/*! writ double-ble number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_double_ble(xu_stream_ref_t stream, xu_double_t value);

/*! writ double-bbe number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_double_bbe(xu_stream_ref_t stream, xu_double_t value);

/*! writ double-lle number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_double_lle(xu_stream_ref_t stream, xu_double_t value);

/*! writ double-lbe number
 *
 * @param stream        the stream
 * @param value         the value
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_stream_bwrit_double_lbe(xu_stream_ref_t stream, xu_double_t value);

#endif
