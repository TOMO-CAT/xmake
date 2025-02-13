#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define XU_ICMP_ECHOREPLY (0)
#define XU_ICMP_ECHOREQ (8)

/* *******************************************************
 * types
 */

// the ip header type - rfc791
typedef struct __xu_ip_header_t
{
    // version and ihl
    xu_uint8_t vihl;

    // type of service
    xu_uint8_t tos;

    // total length of the packet
    xu_uint16_t total_len;

    // unique identifier
    xu_uint16_t id;

    // flags and fragment offset
    xu_uint16_t frag_and_flags;

    // time to live
    xu_uint8_t ttl;

    // the protocol, tcp, udp, ..
    xu_uint8_t proto;

    // the checksum
    xu_uint16_t checksum;

    // the source ipaddr
    xu_uint32_t source_ip;

    // the dest ipaddr
    xu_uint32_t dest_ip;

} __xu_packed__ xu_ip_header_t;

// the icmp header type - rfc792
typedef struct __xu_icmp_header_t
{
    // icmp type
    xu_uint8_t type;

    // type sub code
    xu_uint8_t code;

    // checksum
    xu_uint16_t checksum;

    // identifier
    xu_uint16_t id;

    // sequence numner
    xu_uint16_t seq;

    // data
    xu_byte_t data[1];

} __xu_packed__ xu_icmp_header_t;

// the icmp echo request type
typedef struct __xu_icmp_echo_request_t
{
    // the icmp header
    xu_icmp_header_t icmp;

    // the request time
    xu_uint64_t time;

} __xu_packed__ xu_icmp_echo_request_t;

// the icmp echo reply type
typedef struct __xu_icmp_echo_reply_t
{
    // the ip header
    xu_ip_header_t ip;

    // the echo request
    xu_icmp_echo_request_t request;

} __xu_packed__ xu_icmp_echo_reply_t;

/* *******************************************************
 * implementation
 */
static xu_uint16_t xu_calculate_checksum(xu_byte_t const* data, xu_long_t size)
{
    // calculate sum
    xu_uint32_t      sum   = 0;
    xu_long_t        nleft = size;
    xu_byte_t const* p     = data;
    while (nleft > 1)
    {
        sum += xu_bits_get_u16_le(p);
        nleft -= 2;
        p += 2;
    }

    // calculate answer
    xu_uint16_t answer = 0;
    if (nleft == 1)
    {
        *(xu_byte_t*)(&answer) = *p;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}
static xu_bool_t xu_ping_send(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_uint16_t seq)
{
    // init echo
    xu_icmp_echo_request_t echo;
    xu_uint64_t            time = xu_mclock();
    echo.icmp.type              = XU_ICMP_ECHOREQ;
    echo.icmp.code              = 0;
    xu_bits_set_u16_le((xu_byte_t*)&echo.icmp.checksum, 0);
    xu_bits_set_u16_le((xu_byte_t*)&echo.icmp.id, 0xbeaf);
    xu_bits_set_u16_le((xu_byte_t*)&echo.icmp.seq, seq);
    xu_bits_set_u64_le((xu_byte_t*)&echo.time, time);
    xu_uint16_t checksum = xu_calculate_checksum((xu_byte_t const*)&echo, sizeof(echo));
    xu_bits_set_u16_le((xu_byte_t*)&echo.icmp.checksum, checksum);

    // send echo
    xu_long_t        send = 0;
    xu_long_t        size = sizeof(echo);
    xu_bool_t        wait = xu_false;
    xu_byte_t const* data = (xu_byte_t const*)&echo;
    while (send < size)
    {
        xu_long_t real = xu_socket_usend(sock, addr, data + send, size - send);
        if (real > 0)
        {
            send += real;
            wait = xu_false;
        }
        else if (!real && !wait)
        {
            wait = xu_true;
            real = xu_socket_wait(sock, XU_SOCKET_EVENT_SEND, -1);
            xu_assert_and_check_break(real > 0);
        }
        else
            break;
    }

    // ok?
    return send == size;
}
static xu_bool_t xu_ping_recv(xu_socket_ref_t sock, xu_uint16_t seq)
{
    // recv echo
    xu_icmp_echo_reply_t echo;
    xu_long_t            recv = 0;
    xu_long_t            size = sizeof(echo);
    xu_bool_t            wait = xu_false;
    xu_byte_t*           data = (xu_byte_t*)&echo;
    while (recv < size)
    {
        xu_long_t real = xu_socket_urecv(sock, xu_null, data + recv, size - recv);
        if (real > 0)
        {
            recv += real;
            wait = xu_false;
        }
        else if (!real && !wait)
        {
            wait = xu_true;
            real = xu_socket_wait(sock, XU_SOCKET_EVENT_RECV, -1);
            xu_assert_and_check_break(real > 0);
        }
        else
            break;
    }
    xu_assert_and_check_return_val(recv == size, xu_false);

    // check protocol (icmp)
    xu_assert_and_check_return_val(echo.ip.proto == 1, xu_false);

    // check icmp type
    xu_assert_and_check_return_val(echo.request.icmp.type == XU_ICMP_ECHOREPLY, xu_false);

    // check id
    xu_assert_and_check_return_val(xu_bits_get_u16_le((xu_byte_t const*)&echo.request.icmp.id) == 0xbeaf, xu_false);

    // get source ip address
    xu_ipv4_t source_ip;
    source_ip.u32 = xu_bits_get_u32_le((xu_byte_t const*)&echo.ip.source_ip);

    // trace
    xu_uint64_t time = xu_bits_get_u64_le((xu_byte_t const*)&echo.request.time);
    xu_trace_i("%ld bytes from %{ipv4}: icmp_seq=%d ttl=%d time=%ld ms", size, &source_ip, seq, echo.ip.ttl,
               xu_mclock() - (xu_hong_t)time);

    // ok
    return xu_true;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_ping_main(xu_int_t argc, xu_char_t** argv)
{
    // check
    xu_assert_and_check_return_val(argc == 2 && argv[1], -1);

    // init addr
    xu_ipaddr_t addr;
    if (!xu_dns_looker_done(argv[1], &addr)) return -1;

    // init socket
    xu_socket_ref_t sock = xu_socket_init(XU_SOCKET_TYPE_ICMP, XU_IPADDR_FAMILY_IPV4);
    if (sock)
    {
        // trace
        xu_trace_i("PING %s (%{ipv4}): %d data bytes", argv[1], xu_ipaddr_ipv4(&addr), sizeof(xu_icmp_echo_request_t));

        // send ping
        xu_uint16_t i = 0;
        xu_uint16_t n = 10;
        while (i < n && xu_ping_send(sock, &addr, i))
        {
            // recv ping
            if (!xu_ping_recv(sock, i)) break;
            i++;

            // wait some time
            xu_sleep(1);
        }

        // exit socket
        xu_socket_exit(sock);
    }

    return 0;
}
