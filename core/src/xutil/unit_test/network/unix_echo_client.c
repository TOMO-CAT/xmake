#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */

// timeout
#define XU_DEMO_TIMEOUT (-1)

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_unix_echo_client_main(xu_int_t argc, xu_char_t** argv)
{
    // check
    xu_assert_and_check_return_val(argc == 2, -1);
    // done
    xu_socket_ref_t sock = xu_null;
    do
    {
        // init socket
        sock = xu_socket_init(XU_SOCKET_TYPE_TCP, XU_IPADDR_FAMILY_UNIX);
        xu_assert_and_check_break(sock);

        xu_bool_t  is_abstract = argv[1][0] == '@';
        xu_char_t* path        = is_abstract ? argv[1] + 1 : argv[1];

        // init address
        xu_ipaddr_t addr;
        xu_ipaddr_unix_set_cstr(&addr, path, is_abstract);

        // trace
        xu_trace_i("connecting(%p): %{ipaddr} %s..", sock, &addr, is_abstract ? "(abstract)" : "");

        // connect socket
        xu_long_t ok;
        while (!(ok = xu_socket_connect(sock, &addr)))
        {
            // wait it
            if (xu_socket_wait(sock, XU_SOCKET_EVENT_CONN, XU_DEMO_TIMEOUT) <= 0) break;
        }

        // connect ok?
        xu_check_break(ok > 0);

        // loop
        xu_byte_t data[8192] = {0};
        xu_long_t count      = 10000;
        while (count--)
        {
            // send data
            if (xu_socket_bsend(sock, (xu_byte_t const*)"hello world..", 13))
            {
                // recv data
                if (!xu_socket_brecv(sock, data, 13))
                {
                    // error
                    xu_trace_e("send error!");
                    break;
                }
            }
            else
                break;
        }

        // trace
        xu_trace_d("send(%p): %s %s", sock, data, count <= 0 ? "ok" : "failed");

    } while (0);

    // exit socket
    if (sock) xu_socket_exit(sock);
    sock = xu_null;
    return 0;
}
