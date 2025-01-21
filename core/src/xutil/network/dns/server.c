/* *******************************************************
 * trace
 * *******************************************************
 */

#define XU_TRACE_MODULE_NAME "dns_server"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/dns/server.h"
#include "xutil/algorithm/remove_if.h"
#include "xutil/algorithm/sort.h"
#include "xutil/container/vector.h"
#include "xutil/network/dns/prefix.h"
#include "xutil/platform/cache_time.h"
#include "xutil/platform/socket.h"
#include "xutil/platform/spinlock.h"
#include "xutil/stream/static_stream.h"

/* *******************************************************
 * macros
 * *******************************************************
 */
// the dns server test timeout
#define XU_DNS_SERVER_TEST_TIMEOUT (500)

/* *******************************************************
 * types
 * *******************************************************
 */

// the dns server type
typedef struct __xu_dns_server_t
{
    // the rate
    xu_size_t rate;

    // the addr
    xu_ipaddr_t addr;

} xu_dns_server_t, *xu_dns_server_ref_t;

// the dns server list type
typedef struct __xu_dns_server_list_t
{
    // is sorted?
    xu_bool_t sort;

    // the server list
    xu_vector_ref_t list;

} xu_dns_server_list_t;

/* *******************************************************
 * globals
 * *******************************************************
 */
// the lock
static xu_spinlock_t g_lock = XU_SPINLOCK_INIT;

// the server list
static xu_dns_server_list_t g_list = {0};

/* *******************************************************
 * server
 * *******************************************************
 */

static xu_long_t xu_dns_server_comp(xu_element_ref_t element, xu_cpointer_t litem, xu_cpointer_t ritem)
{
    // check
    xu_assert(litem && ritem);

    // the rate
    xu_size_t lrate = ((xu_dns_server_t const*)litem)->rate;
    xu_size_t rrate = ((xu_dns_server_t const*)ritem)->rate;

    // comp
    return (lrate > rrate ? 1 : (lrate < rrate ? -1 : 0));
}
static xu_long_t xu_dns_server_test(xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(addr && !xu_ipaddr_is_empty(addr), -1);

    // done
    xu_static_stream_t sstream;
    xu_byte_t          rpkt[XU_DNS_RPKT_MAXN];
    xu_size_t          size = 0;
    xu_long_t          rate = -1;
    xu_socket_ref_t    sock = xu_null;
    do
    {
        // trace
        xu_trace_d("test: %{ipaddr}: ..", addr);

        // init sock
        sock = xu_socket_init(XU_SOCKET_TYPE_UDP, xu_ipaddr_family(addr));
        xu_assert_and_check_break(sock);

        // init stream
        xu_static_stream_init(&sstream, rpkt, XU_DNS_RPKT_MAXN);

        // identification number
        xu_static_stream_writ_u16_be(&sstream, XU_DNS_HEADER_MAGIC);

        /* 0x2104: 0 0000 001 0000 0000
         *
         * xu_uint16_t qr     :1;       // query/response flag
         * xu_uint16_t opcode :4;       // purpose of message
         * xu_uint16_t aa     :1;       // authoritive answer
         * xu_uint16_t tc     :1;       // truncated message
         * xu_uint16_t rd     :1;       // recursion desired

         * xu_uint16_t ra     :1;       // recursion available
         * xu_uint16_t z      :1;       // its z! reserved
         * xu_uint16_t ad     :1;       // authenticated data
         * xu_uint16_t cd     :1;       // checking disabled
         * xu_uint16_t rcode  :4;       // response code
         *
         * this is a query
         * this is a standard query
         * not authoritive answer
         * not truncated
         * recursion desired
         *
         * recursion not available! hey we dont have it (lol)
         *
         */
#if 1
        xu_static_stream_writ_u16_be(&sstream, 0x0100);
#else
        xu_static_stream_writ_u1(&sstream, 0);         // this is a query
        xu_static_stream_writ_ubits32(&sstream, 0, 4); // this is a standard query
        xu_static_stream_writ_u1(&sstream, 0);         // not authoritive answer
        xu_static_stream_writ_u1(&sstream, 0);         // not truncated
        xu_static_stream_writ_u1(&sstream, 1);         // recursion desired

        xu_static_stream_writ_u1(&sstream, 0); // recursion not available! hey we dont have it (lol)
        xu_static_stream_writ_u1(&sstream, 0);
        xu_static_stream_writ_u1(&sstream, 0);
        xu_static_stream_writ_u1(&sstream, 0);
        xu_static_stream_writ_ubits32(&sstream, 0, 4);
#endif

        /* we have only one question
         *
         * xu_uint16_t question;        // number of question entries
         * xu_uint16_t answer;          // number of answer entries
         * xu_uint16_t authority;       // number of authority entries
         * xu_uint16_t resource;        // number of resource entries
         *
         */
        xu_static_stream_writ_u16_be(&sstream, 1);
        xu_static_stream_writ_u16_be(&sstream, 0);
        xu_static_stream_writ_u16_be(&sstream, 0);
        xu_static_stream_writ_u16_be(&sstream, 0);

        // set questions, see as xu_dns_question_t
        // name + question1 + question2 + ...
        xu_static_stream_writ_u8(&sstream, 3);
        xu_static_stream_writ_u8(&sstream, 'w');
        xu_static_stream_writ_u8(&sstream, 'w');
        xu_static_stream_writ_u8(&sstream, 'w');
        xu_static_stream_writ_u8(&sstream, 5);
        xu_static_stream_writ_u8(&sstream, 't');
        xu_static_stream_writ_u8(&sstream, 'b');
        xu_static_stream_writ_u8(&sstream, 'o');
        xu_static_stream_writ_u8(&sstream, 'o');
        xu_static_stream_writ_u8(&sstream, 'x');
        xu_static_stream_writ_u8(&sstream, 3);
        xu_static_stream_writ_u8(&sstream, 'o');
        xu_static_stream_writ_u8(&sstream, 'r');
        xu_static_stream_writ_u8(&sstream, 'g');
        xu_static_stream_writ_u8(&sstream, '\0');

        // only one question now.
        xu_static_stream_writ_u16_be(&sstream, 1); // we are requesting the ipv4 address
        xu_static_stream_writ_u16_be(&sstream, 1); // it's internet (lol)

        // size
        size = xu_static_stream_offset(&sstream);
        xu_assert_and_check_break(size);

        // init time
        xu_hong_t time = xu_cache_time_spak();

        // se/nd request
        xu_size_t writ = 0;
        xu_bool_t fail = xu_false;
        while (writ < size)
        {
            // writ data
            xu_long_t real = xu_socket_usend(sock, addr, rpkt + writ, size - writ);

            // trace
            xu_trace_d("writ %ld", real);

            // check
            xu_check_break_state(real >= 0, fail, xu_true);

            // no data?
            if (!real)
            {
                // abort?
                xu_check_break_state(!writ, fail, xu_true);

                // wait
                real = xu_socket_wait(sock, XU_SOCKET_EVENT_SEND, XU_DNS_SERVER_TEST_TIMEOUT);

                // fail or timeout?
                xu_check_break_state(real > 0, fail, xu_true);
            }
            else
                writ += real;
        }

        // failed?
        xu_check_break(!fail);

        // only recv id & answer, 8 bytes
        xu_long_t read = 0;
        while (read < 8)
        {
            // read data
            xu_long_t r = xu_socket_urecv(sock, xu_null, rpkt + read, XU_DNS_RPKT_MAXN - read);

            // trace
            xu_trace_d("read %ld", r);

            // check
            xu_check_break(r >= 0);

            // no data?
            if (!r)
            {
                // end?
                xu_check_break(!read);

                // wait
                r = xu_socket_wait(sock, XU_SOCKET_EVENT_RECV, XU_DNS_SERVER_TEST_TIMEOUT);

                // trace
                xu_trace_d("wait %ld", r);

                // fail or timeout?
                xu_check_break(r > 0);
            }
            else
                read += r;
        }

        // check
        xu_check_break(read >= 8);

        // check protocol
        xu_size_t id = xu_bits_get_u16_be(rpkt);
        xu_check_break(id == XU_DNS_HEADER_MAGIC);

        // check answer
        xu_size_t answer = xu_bits_get_u16_be(rpkt + 6);
        xu_check_break(answer > 0);

        // rate
        rate = (xu_long_t)(xu_cache_time_spak() - time);

        // ok
        xu_trace_d("test: %{ipaddr} ok, rate: %u", addr, rate);

    } while (0);

    // exit sock
    xu_socket_exit(sock);

    // ok
    return rate;
}
static xu_bool_t xu_dns_server_rate(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // the server
    xu_bool_t           ok     = xu_false;
    xu_dns_server_ref_t server = (xu_dns_server_ref_t)item;
    if (server && !server->rate)
    {
        // done
        xu_bool_t done = xu_false;
        do
        {
            // the server rate
            xu_long_t rate = xu_dns_server_test(&server->addr);
            xu_check_break(rate >= 0);

            // save the server rate
            server->rate = rate;

            // ok
            done = xu_true;

        } while (0);

        // failed? remove it
        if (!done) ok = xu_true;
    }

    // ok?
    return ok;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_dns_server_init()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // init list
        if (!g_list.list)
        {
            g_list.list = xu_vector_init(8, xu_element_mem(sizeof(xu_dns_server_t), xu_null, xu_null));
            g_list.sort = xu_false;
        }
        xu_assert_and_check_break(g_list.list);

        // ok
        ok = xu_true;

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);

    // failed? exit it
    if (!ok) xu_dns_server_exit();

    // ok?
    return ok;
}
xu_void_t xu_dns_server_exit()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // exit list
    if (g_list.list) xu_vector_exit(g_list.list);
    g_list.list = xu_null;

    // exit sort
    g_list.sort = xu_false;

    // leave
    xu_spinlock_leave(&g_lock);
}
xu_void_t xu_dns_server_dump()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // dump list
    if (g_list.list)
    {
        // trace
        xu_trace_i("============================================================");
        xu_trace_i("list: %u servers", xu_vector_size(g_list.list));

        // walk
        xu_size_t i = 0;
        xu_size_t n = xu_vector_size(g_list.list);
        for (; i < n; i++)
        {
            xu_dns_server_t const* server = (xu_dns_server_t const*)xu_iterator_item(g_list.list, i);
            if (server)
            {
                // trace
                xu_trace_i("server: %{ipaddr}, rate: %u", &server->addr, server->rate);
            }
        }
    }

    // leave
    xu_spinlock_leave(&g_lock);
}
xu_void_t xu_dns_server_sort()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // done
    xu_vector_ref_t list = xu_null;
    do
    {
        // check
        xu_assert_and_check_break(g_list.list);

        // need sort?
        xu_check_break(!g_list.sort);

        // init element
        xu_element_t element = xu_element_mem(sizeof(xu_dns_server_t), xu_null, xu_null);
        element.comp         = xu_dns_server_comp;

        // init list
        list = xu_vector_init(8, element);
        xu_assert_and_check_break(list);

        // copy list
        xu_vector_copy(list, g_list.list);

    } while (0);

    /* sort ok, only done once sort
     * using the unsorted server list at the other thread if the sort have been not finished
     */
    g_list.sort = xu_true;

    // leave
    xu_spinlock_leave(&g_lock);

    // need sort?
    xu_check_return(list);

    // rate list and remove no-rate servers
    xu_remove_if(list, xu_dns_server_rate, xu_null);

    // sort list
    xu_sort_all(list, xu_null);

    // enter
    xu_spinlock_enter(&g_lock);

    // save the sorted server list
    if (xu_vector_size(list))
        xu_vector_copy(g_list.list, list);
    else
    {
        // no faster server? using the previous server list
        xu_trace_w("no faster server");
    }

    // leave
    xu_spinlock_leave(&g_lock);

    // exit list
    xu_vector_exit(list);
}
xu_size_t xu_dns_server_get(xu_ipaddr_t addr[2])
{
    // check
    xu_assert_and_check_return_val(addr, 0);

    // sort first
    xu_dns_server_sort();

    // enter
    xu_spinlock_enter(&g_lock);

    // done
    xu_size_t ok = 0;
    do
    {
        // check
        xu_assert_and_check_break(g_list.list && g_list.sort);

        // init
        xu_size_t i = 0;
        xu_size_t n = xu_min(xu_vector_size(g_list.list), 2);
        xu_assert_and_check_break(n <= 2);

        // done
        for (; i < n; i++)
        {
            // the dns server
            xu_dns_server_t const* server = (xu_dns_server_t const*)xu_iterator_item(g_list.list, i);
            if (server)
            {
                // save addr
                addr[ok++] = server->addr;
            }
        }

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);

    // trace
    xu_assertf(ok, "no server!");

    // ok?
    return ok;
}
xu_void_t xu_dns_server_add(xu_char_t const* addr)
{
    // check
    xu_assert_and_check_return(addr);

    // init first
    xu_dns_server_init();

    // enter
    xu_spinlock_enter(&g_lock);

    // done
    do
    {
        // check
        xu_assert_and_check_break(g_list.list);

        // init server
        xu_dns_server_t server = {0};
        if (!xu_ipaddr_set(&server.addr, addr, XU_DNS_HOST_PORT, XU_IPADDR_FAMILY_NONE)) break;

        // add server
        xu_vector_insert_tail(g_list.list, &server);

        // need sort it again
        g_list.sort = xu_false;

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);
}
