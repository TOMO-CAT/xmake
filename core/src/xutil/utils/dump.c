#include "xutil/utils/dump.h"
#include "xutil/libc/stdio/stdio.h"
#include "xutil/macros/assert.h"

xu_void_t xu_dump_data(xu_byte_t const* data, xu_size_t size)
{
    // check
    xu_assert_and_check_return(data && size);

    // dump head
    xu_trace_i("");

    // walk
    xu_size_t        i = 0;
    xu_size_t        n = 147;
    xu_byte_t const* p = data;
    xu_byte_t const* e = data + size;
    xu_char_t        info[8192];
    while (p < e)
    {
        // full line?
        xu_char_t* q = info;
        xu_char_t* d = info + sizeof(info);
        if (p + 0x20 <= e)
        {
            // dump offset
            if (q < d) q += xu_snprintf(q, d - q, "%08X ", p - data);

            // dump data
            for (i = 0; i < 0x20; i++)
            {
                if (!(i & 3) && q < d) q += xu_snprintf(q, d - q, " ");
                if (q < d) q += xu_snprintf(q, d - q, " %02X", p[i]);
            }

            // dump spaces
            if (q < d) q += xu_snprintf(q, d - q, "  ");

            // dump characters
            for (i = 0; i < 0x20; i++)
            {
                if (q < d) q += xu_snprintf(q, d - q, "%c", xu_isgraph(p[i]) ? p[i] : '.');
            }

            // dump it
            if (q < d)
            {
                // end
                *q = '\0';

                // trace
                xu_trace_i("%s", info);
            }

            // update p
            p += 0x20;
        }
        // has left?
        else if (p < e)
        {
            // init padding
            xu_size_t padding = n - 0x20;

            // dump offset
            if (q < d) q += xu_snprintf(q, d - q, "%08X ", p - data);
            if (padding >= 9) padding -= 9;

            // dump data
            xu_size_t left = e - p;
            for (i = 0; i < left; i++)
            {
                if (!(i & 3))
                {
                    if (q < d) q += xu_snprintf(q, d - q, " ");
                    if (padding) padding--;
                }

                if (q < d) q += xu_snprintf(q, d - q, " %02X", p[i]);
                if (padding >= 3) padding -= 3;
            }

            // dump spaces
            while (padding--)
                if (q < d) q += xu_snprintf(q, d - q, " ");

            // dump characters
            for (i = 0; i < left; i++)
            {
                if (q < d) q += xu_snprintf(q, d - q, "%c", xu_isgraph(p[i]) ? p[i] : '.');
            }

            // dump it
            if (q < d)
            {
                // end
                *q = '\0';

                // trace
                xu_trace_i("%s", info);
            }

            // update p
            p += left;
        }
        // end
        else
            break;
    }
}
