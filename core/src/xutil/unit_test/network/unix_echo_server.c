#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */

// timeout
#define XU_DEMO_TIMEOUT (-1)

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_unix_echo_server_main(xu_int_t argc, xu_char_t** argv)
{
    // check
    xu_assert_and_check_return_val(argc == 2, -1);

    // get path
    xu_bool_t  is_abstract = argv[1][0] == '@';
    xu_char_t* path        = is_abstract ? argv[1] + 1 : argv[1];

    // done
    xu_socket_ref_t sock = xu_null;
    do
    {
        // init socket
        sock = xu_socket_init(XU_SOCKET_TYPE_TCP, XU_IPADDR_FAMILY_UNIX);
        xu_assert_and_check_break(sock);

        if (!is_abstract)
        {
            // clear old socket
            xu_file_remove(path);
        }

        // bind socket
        xu_ipaddr_t addr;
        xu_ipaddr_unix_set_cstr(&addr, path, is_abstract);
        if (!xu_socket_bind(sock, &addr)) break;

        // listen socket
        if (!xu_socket_listen(sock, 1000)) break;

        // trace
        xu_trace_i("listening %{ipaddr} %s..", &addr, is_abstract ? "(abstract)" : "");

        // accept client sockets
        xu_socket_ref_t client = xu_null;
        while (1)
        {
            // accept and start client connection
            if ((client = xu_socket_accept(sock, xu_null)))
            {
                // loop
                xu_byte_t data[8192] = {0};
                xu_size_t size       = 13;
                while (1)
                {
                    // recv data
                    if (xu_socket_brecv(client, data, size))
                    {
                        // send data
                        if (!xu_socket_bsend(client, data, size))
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
                xu_trace_d("echo: %s", data);

                // exit socket
                xu_socket_exit(client);
            }
            else if (xu_socket_wait(sock, XU_SOCKET_EVENT_ACPT, -1) <= 0)
                break;
        }
    } while (0);

    // exit socket
    if (sock) xu_socket_exit(sock);
    sock = xu_null;
    return 0;
}
