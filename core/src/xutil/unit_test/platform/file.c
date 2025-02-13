#include "xutil/unit_test/ut.h"

/* *******************************************************
 * main
 */
xu_int_t xu_ut_platform_file_main(xu_int_t argc, xu_char_t** argv)
{
#if 0
    // init file
//  xu_file_ref_t file = xu_file_init(argv[1], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    xu_file_ref_t file = xu_file_init(argv[1], XU_FILE_MODE_DIRECT | XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (file)
    {
        // done
        xu_size_t   align = XU_FILE_DIRECT_ASIZE;
        xu_size_t   writ = 0;
        xu_size_t   size = 512 * 1024 * 1024;
        xu_size_t   maxn = 8096;
//      xu_size_t   maxn = (1 << 17);
        xu_byte_t*  data = xu_malloc(maxn + align);
        xu_hong_t   time = xu_mclock();
        if (data)
        {
            // align
            xu_byte_t* buff = (xu_byte_t*)xu_align((xu_hize_t)data, align);

            // writ file
            while (writ < size)
            {
                xu_long_t real = xu_file_writ(file, buff, xu_min(maxn, size - writ));
//              xu_trace_i("real: %ld, size: %lu", real, xu_min(maxn, size - writ));
                if (real > 0) writ += real;
                else if (!real) {}
                else break;
            }

            // exit data
            xu_free(data);
        }

        // sync
        xu_file_sync(file);

        // ok
        if (writ == size)
        {
            // trace
            time = xu_mclock() - time;
            xu_trace_i("writ: %lld s", time / 1000);
            time = xu_mclock();
        }

        // exit file
        xu_file_exit(file);
    }
#elif 0
    // init file
    xu_file_ref_t file = xu_file_init(argv[1], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (file)
    {
        // done
        xu_size_t  writ = 0;
        xu_size_t  size = 512 * 1024 * 1024;
        xu_size_t  maxn = 8096;
        xu_iovec_t list[4];
        xu_hong_t  time = xu_mclock();
        xu_byte_t* data = xu_malloc(maxn << 2);

        // init iovec
        list[0].data = xu_malloc(maxn);
        list[1].data = xu_malloc(maxn);
        list[2].data = xu_malloc(maxn);
        list[3].data = xu_malloc(maxn);
        list[0].size = maxn;
        list[1].size = maxn;
        list[2].size = maxn;
        list[3].size = maxn;

        // writ file
        while (writ < size)
        {
            // size
            xu_memcpy(data, list[0].data, list[0].size);
            xu_memcpy(data + maxn, list[1].data, list[1].size);
            xu_memcpy(data + maxn + maxn, list[2].data, list[2].size);
            xu_memcpy(data + maxn + maxn + maxn, list[3].data, list[3].size);

            // writ
            xu_long_t real = xu_file_writ(file, data, xu_min((size - writ), (maxn << 2)));
            if (real > 0)
                writ += real;
            else if (!real)
            {
            }
            else
                break;
        }

        // exit data
        xu_free(list[0].data);
        xu_free(list[1].data);
        xu_free(list[2].data);
        xu_free(list[3].data);
        xu_free(data);

        // sync
        xu_file_sync(file);

        // ok
        if (writ == size)
        {
            // trace
            time = xu_mclock() - time;
            xu_trace_i("writ: %lld s", time / 1000);
            time = xu_mclock();
        }

        // exit file
        xu_file_exit(file);
    }
#elif 0
    // init file
    xu_file_ref_t file = xu_file_init(argv[1], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (file)
    {
        // done
        xu_size_t  writ = 0;
        xu_size_t  size = 512 * 1024 * 1024;
        xu_size_t  maxn = 8096;
        xu_iovec_t list[4];
        xu_hong_t  time = xu_mclock();

        // init iovec
        list[0].data = xu_malloc(maxn);
        list[1].data = xu_malloc(maxn);
        list[2].data = xu_malloc(maxn);
        list[3].data = xu_malloc(maxn);
        list[0].size = maxn;
        list[1].size = maxn;
        list[2].size = maxn;
        list[3].size = maxn;

        // writ file
        while (writ < size)
        {
            // size
            xu_size_t left = size - writ;
            if (left < (maxn << 2))
            {
                list[0].size = left >= maxn ? maxn : left;
                left -= list[0].size;
                list[1].size = left >= maxn ? maxn : left;
                left -= list[1].size;
                list[2].size = left >= maxn ? maxn : left;
                left -= list[2].size;
                list[3].size = left >= maxn ? maxn : left;
                left -= list[3].size;
            }

            // writ
            xu_long_t real = xu_file_writv(file, list, 4);
            if (real > 0)
                writ += real;
            else if (!real)
            {
            }
            else
                break;
        }

        // exit data
        xu_free(list[0].data);
        xu_free(list[1].data);
        xu_free(list[2].data);
        xu_free(list[3].data);

        // sync
        xu_file_sync(file);

        // ok
        if (writ == size)
        {
            // trace
            time = xu_mclock() - time;
            xu_trace_i("writ: %lld s", time / 1000);
            time = xu_mclock();
        }

        // exit file
        xu_file_exit(file);
    }
#elif 0
    xu_file_ref_t ifile = xu_file_init(argv[1], XU_FILE_MODE_RW);
    xu_file_ref_t ofile = xu_file_init(argv[2], XU_FILE_MODE_RW | XU_FILE_MODE_CREAT | XU_FILE_MODE_TRUNC);
    if (ifile && ofile)
    {
        xu_hize_t writ = 0;
        xu_hize_t size = xu_file_size(ifile);
        while (writ < size)
        {
            xu_long_t real = xu_file_writf(ofile, ifile, writ, size - writ);
            if (real > 0)
                writ += real;
            else
                break;
        }
    }

    // exit file
    if (ifile) xu_file_exit(ifile);
    if (ofile) xu_file_exit(ofile);
#elif 1
    xu_bool_t ok = xu_file_rename(argv[1], argv[2]);
    xu_trace_i("rename: %s => %s %s", argv[1], argv[2], ok ? "ok" : "failed");
#elif 1
    xu_long_t ok = xu_file_fscase(argv[1]);
    xu_trace_i("fscase: %s => %d", argv[1], ok);
#elif 0
    xu_bool_t ok = xu_file_copy(argv[1], argv[2], XU_FILE_COPY_LINK);
    xu_trace_i("copy: %s => %s %s", argv[1], argv[2], ok ? "ok" : "failed");
#elif 0
    xu_file_info_t info;
    xu_hong_t      time = xu_time();
    if (xu_file_touch(argv[1], time, time) && xu_file_info(argv[1], &info))
        xu_trace_i("touch: %s, atime: %lld mtime: %lld, time: %lld", argv[1], info.atime, info.mtime, time);
#elif 0
    xu_bool_t ok = xu_file_link(argv[1], argv[2]);
    xu_trace_i("link: %s => %s %s", argv[1], argv[2], ok ? "ok" : "failed");
#else
    // remove
    xu_file_remove(argv[1]);
#endif

    return 0;
}
