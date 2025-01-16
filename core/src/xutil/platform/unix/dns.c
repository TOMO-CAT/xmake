#include "xutil/platform/impl/dns.h"
#include "xutil/platform/file.h"
#include "xutil/stream/stream.h"

/* *******************************************************
 *  interfaces
 * *******************************************************
 */
xu_bool_t xu_dns_init_env()
{
    if (xu_file_info("/etc/resolv.conf", xu_null))
    {
        /* try get list from "/etc/resolv.conf"
         *
         * # Generated by NetworkManager
         * nameserver 10.1.20.10
         * nameserver 8.8.8.8
         *
         */
        xu_stream_ref_t stream = xu_stream_init_from_url("/etc/resolv.conf");
        if (stream)
        {
            // open
            if (xu_stream_open(stream))
            {
                // read
                xu_long_t size = 0;
                xu_char_t line[8192];
                while ((size = xu_stream_bread_line(stream, line, 8192)) >= 0)
                {
                    if (size && !xu_strnicmp(line, "nameserver", 10))
                    {
                        // seek to server
                        xu_char_t const* p = line + 10;
                        while (*p && !xu_isdigit(*p))
                            p++;
                        xu_check_continue(*p);

                        // add server
                        xu_dns_server_add(p);
                    }
                }
            }

            // exit
            xu_stream_exit(stream);
        }
    }

    // ok
    return xu_true;
}
xu_void_t xu_dns_exit_env() {}
