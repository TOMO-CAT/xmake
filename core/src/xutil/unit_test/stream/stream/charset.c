#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
#ifdef XU_CONFIG_MODULE_HAVE_CHARSET
xu_int_t xu_ut_stream_charset_main(xu_int_t argc, xu_char_t** argv)
{
    // init istream
    xu_stream_ref_t istream = xu_stream_init_from_url(argv[1]);

    // init ostream
    xu_stream_ref_t ostream =
        xu_stream_init_from_file(argv[2], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);

    // filter istream or ostream?
    xu_stream_ref_t iostream = istream;
    //  xu_stream_ref_t iostream = ostream;

    // init fstream
    xu_stream_ref_t fstream =
        xu_stream_init_filter_from_charset(iostream, xu_charset_type(argv[3]), xu_charset_type(argv[4]));

    // done
    if (istream && ostream && fstream)
    {
        // save it
        xu_hong_t save = 0;
        if (iostream == istream)
            save = xu_transfer(fstream, ostream, 0, xu_null, xu_null);
        else
            save = xu_transfer(istream, fstream, 0, xu_null, xu_null);

        // trace
        xu_trace_i("save: %lld bytes, size: %lld bytes", save, xu_stream_size(istream));
    }

    // exit fstream
    xu_stream_exit(fstream);

    // exit istream
    xu_stream_exit(istream);

    // exit ostream
    xu_stream_exit(ostream);
    return 0;
}
#else
xu_int_t xu_ut_stream_charset_main(xu_int_t argc, xu_char_t** argv) { return 0; }
#endif
