/* *******************************************************
 * trace
 */

#define XU_TRACE_MODULE_NAME "transfer"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/stream/transfer.h"
#include "xutil/macros/assert.h"
#include "xutil/platform/cache_time.h"
#include "xutil/platform/file.h"
#include "xutil/platform/time.h"
#include "xutil/stream/stream.h"
#include "xutil/utils/state.h"

/* *******************************************************
 * interfaces
 */
xu_hong_t xu_transfer(xu_stream_ref_t istream, xu_stream_ref_t ostream, xu_size_t lrate, xu_transfer_func_t func,
                      xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(ostream && istream, -1);

    // open it first if istream have been not opened
    if (xu_stream_is_closed(istream) && !xu_stream_open(istream)) return -1;

    // open it first if ostream have been not opened
    if (xu_stream_is_closed(ostream) && !xu_stream_open(ostream)) return -1;

    // done func
    if (func) func(XU_STATE_OK, xu_stream_offset(istream), xu_stream_size(istream), 0, 0, priv);

    // writ data
    xu_byte_t data[XU_STREAM_BLOCK_MAXN];
    xu_hize_t writ   = 0;
    xu_hize_t left   = xu_stream_left(istream);
    xu_hong_t base   = xu_cache_time_spak();
    xu_hong_t base1s = base;
    xu_hong_t time   = 0;
    xu_size_t crate  = 0;
    xu_long_t delay  = 0;
    xu_size_t writ1s = 0;
    do
    {
        // the need
        xu_size_t need = lrate ? xu_min(lrate, XU_STREAM_BLOCK_MAXN) : XU_STREAM_BLOCK_MAXN;

        // read data
        xu_long_t real = xu_stream_read(istream, data, need);
        if (real > 0)
        {
            // writ data
            if (!xu_stream_bwrit(ostream, data, real)) break;

            // save writ
            writ += real;

            // has func or limit rate?
            if (func || lrate)
            {
                // the time
                time = xu_cache_time_spak();

                // < 1s?
                if (time < base1s + 1000)
                {
                    // save writ1s
                    writ1s += real;

                    // save current rate if < 1s from base
                    if (time < base + 1000) crate = writ1s;

                    // compute the delay for limit rate
                    if (lrate) delay = writ1s >= lrate ? (xu_size_t)(base1s + 1000 - time) : 0;
                }
                else
                {
                    // save current rate
                    crate = writ1s;

                    // update base1s
                    base1s = time;

                    // reset writ1s
                    writ1s = 0;

                    // reset delay
                    delay = 0;

                    // done func
                    if (func) func(XU_STATE_OK, xu_stream_offset(istream), xu_stream_size(istream), writ, crate, priv);
                }

                // wait some time for limit rate
                if (delay) xu_msleep(delay);
            }
        }
        else if (!real)
        {
            // wait
            xu_long_t wait = xu_stream_wait(istream, XU_STREAM_WAIT_READ, xu_stream_timeout(istream));
            xu_check_break(wait > 0);

            // has writ?
            xu_assert_and_check_break(wait & XU_STREAM_WAIT_READ);
        }
        else
            break;

        // is end?
        if (writ >= left) break;

    } while (1);

    // sync the ostream
    if (!xu_stream_sync(ostream, xu_true)) return -1;

    // has func?
    if (func)
    {
        // the time
        time = xu_cache_time_spak();

        // compute the total rate
        xu_size_t trate = (writ && (time > base)) ? (xu_size_t)((writ * 1000) / (time - base)) : (xu_size_t)writ;

        // done func
        func(XU_STATE_CLOSED, xu_stream_offset(istream), xu_stream_size(istream), writ, trate, priv);
    }

    // ok?
    return writ;
}
xu_hong_t xu_transfer_to_url(xu_stream_ref_t istream, xu_char_t const* ourl, xu_size_t lrate, xu_transfer_func_t func,
                             xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(istream && ourl, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // init ostream
        ostream = xu_stream_init_from_url(ourl);
        xu_assert_and_check_break(ostream);

        // ctrl file
        if (xu_stream_type(ostream) == XU_STREAM_TYPE_FILE)
        {
            // ctrl mode
            if (!xu_stream_ctrl(ostream, XU_STREAM_CTRL_FILE_SET_MODE,
                                XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC))
                break;
        }

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_to_data(xu_stream_ref_t istream, xu_byte_t* odata, xu_size_t osize, xu_size_t lrate,
                              xu_transfer_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(istream && odata && osize, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // init ostream
        ostream = xu_stream_init_from_data(odata, osize);
        xu_assert_and_check_break(ostream);

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_url(xu_char_t const* iurl, xu_char_t const* ourl, xu_size_t lrate, xu_transfer_func_t func,
                          xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(iurl && ourl, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t istream = xu_null;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // copy file to file?
        if (xu_url_protocol_probe(iurl) == XU_URL_PROTOCOL_FILE && xu_url_protocol_probe(ourl) == XU_URL_PROTOCOL_FILE)
        {
            // copy it directly
            xu_file_info_t info;
            if (xu_file_copy(iurl, ourl, XU_FILE_COPY_NONE) && xu_file_info(ourl, &info)) size = info.size;

            // end
            break;
        }

        // init istream
        istream = xu_stream_init_from_url(iurl);
        xu_assert_and_check_break(istream);

        // init ostream
        ostream = xu_stream_init_from_url(ourl);
        xu_assert_and_check_break(ostream);

        // ctrl file
        if (xu_stream_type(ostream) == XU_STREAM_TYPE_FILE)
        {
            // ctrl mode
            if (!xu_stream_ctrl(ostream, XU_STREAM_CTRL_FILE_SET_MODE,
                                XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC))
                break;
        }

        // open istream
        if (!xu_stream_open(istream)) break;

        // open ostream
        if (!xu_stream_open(ostream)) break;

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_url_to_stream(xu_char_t const* iurl, xu_stream_ref_t ostream, xu_size_t lrate,
                                    xu_transfer_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(iurl && ostream, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t istream = xu_null;
    do
    {
        // init istream
        istream = xu_stream_init_from_url(iurl);
        xu_assert_and_check_break(istream);

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_url_to_data(xu_char_t const* iurl, xu_byte_t* odata, xu_size_t osize, xu_size_t lrate,
                                  xu_transfer_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(iurl && odata && osize, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t istream = xu_null;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // init istream
        istream = xu_stream_init_from_url(iurl);
        xu_assert_and_check_break(istream);

        // init ostream
        ostream = xu_stream_init_from_data(odata, osize);
        xu_assert_and_check_break(ostream);

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_data_to_url(xu_byte_t const* idata, xu_size_t isize, xu_char_t const* ourl, xu_size_t lrate,
                                  xu_transfer_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(idata && isize && ourl, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t istream = xu_null;
    xu_stream_ref_t ostream = xu_null;
    do
    {
        // init istream
        istream = xu_stream_init_from_data(idata, isize);
        xu_assert_and_check_break(istream);

        // init ostream
        ostream = xu_stream_init_from_url(ourl);
        xu_assert_and_check_break(ostream);

        // ctrl file
        if (xu_stream_type(ostream) == XU_STREAM_TYPE_FILE)
        {
            // ctrl mode
            if (!xu_stream_ctrl(ostream, XU_STREAM_CTRL_FILE_SET_MODE,
                                XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC))
                break;
        }

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // exit ostream
    if (ostream) xu_stream_exit(ostream);
    ostream = xu_null;

    // ok?
    return size;
}
xu_hong_t xu_transfer_data_to_stream(xu_byte_t const* idata, xu_size_t isize, xu_stream_ref_t ostream, xu_size_t lrate,
                                     xu_transfer_func_t func, xu_cpointer_t priv)
{
    // check
    xu_assert_and_check_return_val(idata && isize && ostream, -1);

    // done
    xu_hong_t       size    = -1;
    xu_stream_ref_t istream = xu_null;
    do
    {
        // init istream
        istream = xu_stream_init_from_data(idata, isize);
        xu_assert_and_check_break(istream);

        // save stream
        size = xu_transfer(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) xu_stream_exit(istream);
    istream = xu_null;

    // ok?
    return size;
}
