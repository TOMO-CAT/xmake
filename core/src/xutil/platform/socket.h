#pragma once

#include "xutil/macros/assert.h"
#include "xutil/network/ipaddr.h"
#include "xutil/platform/prefix.h"
#include "xutil/platform/socket.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the socket type enum
typedef enum __xu_socket_type_e
{
    XU_SOCKET_TYPE_NONE = 0

    // socket types
    ,
    XU_SOCKET_TYPE_SOCK_STREAM = 1 << 8,
    XU_SOCKET_TYPE_SOCK_DGRAM  = 2 << 8,
    XU_SOCKET_TYPE_SOCK_RAW    = 3 << 8

    // socket ip protocol
    ,
    XU_SOCKET_TYPE_IPPROTO_TCP  = 1,
    XU_SOCKET_TYPE_IPPROTO_UDP  = 2,
    XU_SOCKET_TYPE_IPPROTO_ICMP = 3

    // socket for tcp
    ,
    XU_SOCKET_TYPE_TCP = XU_SOCKET_TYPE_SOCK_STREAM | XU_SOCKET_TYPE_IPPROTO_TCP

    // socket for udp
    ,
    XU_SOCKET_TYPE_UDP = XU_SOCKET_TYPE_SOCK_DGRAM | XU_SOCKET_TYPE_IPPROTO_UDP

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_IOS)
    // socket for icmp, only need user permission on macOS/iOS
    ,
    XU_SOCKET_TYPE_ICMP = XU_SOCKET_TYPE_SOCK_DGRAM | XU_SOCKET_TYPE_IPPROTO_ICMP
#else
    // socket for icmp, need root permission on linux
    ,
    XU_SOCKET_TYPE_ICMP = XU_SOCKET_TYPE_SOCK_RAW | XU_SOCKET_TYPE_IPPROTO_ICMP
#endif

} xu_socket_type_e;

// the socket kill enum
typedef enum __xu_socket_kill_e
{
    XU_SOCKET_KILL_RO = 0,
    XU_SOCKET_KILL_WO = 1,
    XU_SOCKET_KILL_RW = 2

} xu_socket_kill_e;

// the socket ctrl enum
typedef enum __xu_socket_ctrl_e
{
    XU_SOCKET_CTRL_SET_BLOCK          = 0,
    XU_SOCKET_CTRL_GET_BLOCK          = 1,
    XU_SOCKET_CTRL_SET_RECV_BUFF_SIZE = 2,
    XU_SOCKET_CTRL_GET_RECV_BUFF_SIZE = 3,
    XU_SOCKET_CTRL_SET_SEND_BUFF_SIZE = 4,
    XU_SOCKET_CTRL_GET_SEND_BUFF_SIZE = 5,
    XU_SOCKET_CTRL_SET_TCP_NODELAY    = 6,
    XU_SOCKET_CTRL_GET_TCP_NODELAY    = 7,
    XU_SOCKET_CTRL_SET_TCP_KEEPINTVL  = 8,
    XU_SOCKET_CTRL_SET_KEEPALIVE      = 9,
    XU_SOCKET_CTRL_SET_NOSIGPIPE      = 10

} xu_socket_ctrl_e;

// the socket event enum, only for sock
typedef enum __xu_socket_event_e
{
    XU_SOCKET_EVENT_NONE = 0x0000,
    XU_SOCKET_EVENT_RECV = 0x0001,
    XU_SOCKET_EVENT_SEND = 0x0002,
    XU_SOCKET_EVENT_CONN = XU_SOCKET_EVENT_SEND,
    XU_SOCKET_EVENT_ACPT = XU_SOCKET_EVENT_RECV,
    XU_SOCKET_EVENT_EALL = XU_SOCKET_EVENT_RECV | XU_SOCKET_EVENT_SEND

} xu_socket_event_e;

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! init socket
 *
 * @param type      the socket type
 * @param family    the address family, default: ipv4
 *
 * @return          the socket
 */
xu_socket_ref_t xu_socket_init(xu_size_t type, xu_size_t family);

/*! exit socket
 *
 * @param sock      the socket
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_exit(xu_socket_ref_t sock);

/*! init socket pair
 *
 * @param type      the socket type
 * @param pair      the socket pair
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_pair(xu_size_t type, xu_socket_ref_t pair[2]);

/*! ctrl the socket
 *
 * @param sock      the socket
 * @param ctrl      the ctrl code
 */
xu_bool_t xu_socket_ctrl(xu_socket_ref_t sock, xu_size_t ctrl, ...);

/*! connect the given client address
 *
 * @param sock      the socket
 * @param addr      the client address
 *
 * @return          ok: 1, continue: 0; failed: -1
 */
xu_long_t xu_socket_connect(xu_socket_ref_t sock, xu_ipaddr_ref_t addr);

/*! bind socket
 *
 * you can call xu_socket_local for the bound address
 *
 * @param sock      the socket
 * @param addr      the address
 *                  - bind any port if port == 0
 *                  - bind any ip address if ip is empty
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_bind(xu_socket_ref_t sock, xu_ipaddr_ref_t addr);

/*! listen socket
 *
 * @param sock      the socket
 * @param backlog   the maximum length for the queue of pending connections
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_listen(xu_socket_ref_t sock, xu_size_t backlog);

/*! accept socket
 *
 * @param sock      the socket
 * @param addr      the client address
 *
 * @return          the client socket
 */
xu_socket_ref_t xu_socket_accept(xu_socket_ref_t sock, xu_ipaddr_ref_t addr);

/*! get local address
 *
 * @param sock      the socket
 * @param addr      the local address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_local(xu_socket_ref_t sock, xu_ipaddr_ref_t addr);

/*! get peer address
 *
 * @param sock      the socket
 * @param addr      the peer address
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_peer(xu_socket_ref_t sock, xu_ipaddr_ref_t addr);

/*! kill socket
 *
 * @param sock      the socket
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_kill(xu_socket_ref_t sock, xu_size_t mode);

/*! recv the socket data for tcp
 *
 * @param sock      the socket
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_recv(xu_socket_ref_t sock, xu_byte_t* data, xu_size_t size);

/*! send the socket data for tcp
 *
 * @param sock      the socket
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_send(xu_socket_ref_t sock, xu_byte_t const* data, xu_size_t size);

/*! recv the socket data for tcp with block mode
 *
 * @param sock      the socket
 * @param data      the data
 * @param size      the size
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_brecv(xu_socket_ref_t sock, xu_byte_t* data, xu_size_t size);

/*! send the socket data for tcp with block mode
 *
 * @param sock      the socket
 * @param data      the data
 * @param size      the size
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_socket_bsend(xu_socket_ref_t sock, xu_byte_t const* data, xu_size_t size);

/*! recvv the socket data for tcp
 *
 * @param sock      the socket
 * @param list      the iovec list
 * @param size      the iovec size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_recvv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size);

/*! sendv the socket data for tcp
 *
 * @param sock      the socket
 * @param list      the iovec list
 * @param size      the iovec size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_sendv(xu_socket_ref_t sock, xu_iovec_t const* list, xu_size_t size);

/*! send file data
 *
 * @param sock      the socket
 * @param file      the file
 * @param offset    the offset
 * @param size      the size
 *
 * @return          the real size or -1
 */
xu_hong_t xu_socket_sendf(xu_socket_ref_t sock, xu_file_ref_t file, xu_hize_t offset, xu_hize_t size);

/*! send the socket data for udp
 *
 * @param sock      the socket
 * @param addr      the address
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_usend(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t const* data, xu_size_t size);

/*! recv the socket data for udp
 *
 * @param sock      the socket
 * @param addr      the peer address(output)
 * @param data      the data
 * @param size      the size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_urecv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_byte_t* data, xu_size_t size);

/*! urecvv the socket data for udp
 *
 * @param sock      the socket
 * @param addr      the peer address(output)
 * @param list      the iovec list
 * @param size      the iovec size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_urecvv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size);

/*! usendv the socket data for udp
 *
 * @param sock      the socket
 * @param addr      the addr
 * @param list      the iovec list
 * @param size      the iovec size
 *
 * @return          the real size or -1
 */
xu_long_t xu_socket_usendv(xu_socket_ref_t sock, xu_ipaddr_ref_t addr, xu_iovec_t const* list, xu_size_t size);

/*! wait socket events
 *
 * @note we can wait for socket events in the coroutine
 *
 * @param sock      the sock
 * @param events    the socket events
 * @param timeout   the timeout, infinity: -1
 *
 * @return          > 0: the events code, 0: timeout or interrupted, -1: failed
 */
xu_long_t xu_socket_wait(xu_socket_ref_t sock, xu_size_t events, xu_long_t timeout);
