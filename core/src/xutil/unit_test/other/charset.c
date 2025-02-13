#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_other_charset_main(xu_int_t argc, xu_char_t** argv)
{
    // check
    xu_assert_and_check_return_val(argc == 5, 0);

    // init stream
    xu_stream_ref_t istream = xu_stream_init_from_url(argv[1]);
    xu_stream_ref_t ostream =
        xu_stream_init_from_file(argv[2], XU_FILE_MODE_WO | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (istream && ostream && xu_stream_open(istream) && xu_stream_open(ostream))
    {
        // init data & size
        xu_hong_t isize = xu_stream_size(istream);
        if (isize > 0)
        {
            xu_long_t  osize = (xu_long_t)(isize << 2);
            xu_byte_t* idata = xu_malloc_bytes((xu_size_t)isize);
            xu_byte_t* odata = xu_malloc_bytes((xu_size_t)osize);
            if (idata && odata && xu_stream_bread(istream, idata, (xu_size_t)isize))
            {
                // conv
                osize = xu_charset_conv_data(xu_charset_type(argv[3]), xu_charset_type(argv[4]), idata,
                                             (xu_size_t)isize, odata, osize);
                xu_trace_i("conv: %ld bytes", osize);

                // save
                if (osize > 0) xu_stream_bwrit(ostream, odata, osize);
            }

            // exit data
            if (idata) xu_free(idata);
            if (odata) xu_free(odata);
        }

        // exit stream
        xu_stream_exit(istream);
        xu_stream_exit(ostream);
    }

    return 0;
}
