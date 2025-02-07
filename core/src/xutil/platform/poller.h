#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/platform/fwatcher.h"
#include "xutil/platform/pipe.h"
#include "xutil/platform/process.h"
#include "xutil/platform/socket.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the poller type enum
typedef enum __xu_poller_type_e
{
    XU_POLLER_TYPE_NONE   = 0,
    XU_POLLER_TYPE_IOCP   = 1,
    XU_POLLER_TYPE_POLL   = 2,
    XU_POLLER_TYPE_EPOLL  = 3,
    XU_POLLER_TYPE_KQUEUE = 4,
    XU_POLLER_TYPE_SELECT = 5

} xu_poller_type_e;

// the poller event enum, only for sock
typedef enum __xu_poller_event_e
{
    // the waited events
    XU_POLLER_EVENT_NONE = XU_SOCKET_EVENT_NONE,
    XU_POLLER_EVENT_CONN = XU_SOCKET_EVENT_CONN,
    XU_POLLER_EVENT_ACPT = XU_SOCKET_EVENT_ACPT,
    XU_POLLER_EVENT_RECV = XU_SOCKET_EVENT_RECV,
    XU_POLLER_EVENT_SEND = XU_SOCKET_EVENT_SEND,
    XU_POLLER_EVENT_EALL = XU_SOCKET_EVENT_EALL

    // the event flags after waiting
    ,
    XU_POLLER_EVENT_CLEAR = 0x0010 //!< edge trigger. after the event is retrieved by the user, its state is reset
    ,
    XU_POLLER_EVENT_ONESHOT =
        0x0020 //!< causes the event to return only the first occurrence of the filter being triggered
    ,
    XU_POLLER_EVENT_NOEXTRA = 0x0040 //!< do not pass and storage the extra userdata for events

    /*! the event flag will be marked if the connection be closed in the edge trigger (XU_POLLER_EVENT_CLEAR)
     *
     * be similar to epoll.EPOLLRDHUP and kqueue.EV_EOF
     */
    ,
    XU_POLLER_EVENT_EOF = 0x0100

    // socket error after waiting
    ,
    XU_POLLER_EVENT_ERROR = 0x0200

} xu_poller_event_e;

// the poller object type enum
typedef enum __xu_poller_object_type_e
{
    XU_POLLER_OBJECT_NONE = 0,
    XU_POLLER_OBJECT_SOCK = 1 //!< socket
    ,
    XU_POLLER_OBJECT_PIPE = 2 //!< pipe
    ,
    XU_POLLER_OBJECT_PROC = 3 //!< process
    ,
    XU_POLLER_OBJECT_FWATCHER = 4 //!< fwatcher

} xu_poller_object_type_e;

// the poller ref type
typedef __xu_typeref__(poller);

// the poller object type
typedef struct __xu_poller_object_t
{
    // the object type
    xu_uint8_t type;

    // the object reference
    union
    {
        xu_socket_ref_t    sock;
        xu_pipe_file_ref_t pipe;
        xu_process_ref_t   proc;
        xu_fwatcher_ref_t  fwatcher;
        xu_pointer_t       ptr;

    } ref;

} xu_poller_object_t, *xu_poller_object_ref_t;

/*! the poller event func type
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events or process status
 * @param priv      the user private data for this socket
 */
typedef xu_void_t (*xu_poller_event_func_t)(xu_poller_ref_t poller, xu_poller_object_ref_t object, xu_long_t events,
                                            xu_cpointer_t priv);

/* *******************************************************
 * interfaces
 */

/*! init poller
 *
 * @param priv      the user private data
 *
 * @return          the poller
 */
xu_poller_ref_t xu_poller_init(xu_cpointer_t priv);

/*! exit poller
 *
 * @param poller    the poller
 */
xu_void_t xu_poller_exit(xu_poller_ref_t poller);

/*! get the poller type
 *
 * @param poller    the poller
 *
 * @return          the poller type
 */
xu_size_t xu_poller_type(xu_poller_ref_t poller);

/*! get the user private data
 *
 * @param poller    the poller
 *
 * @return          the user private data
 */
xu_cpointer_t xu_poller_priv(xu_poller_ref_t poller);

/*! kill all waited events, xu_poller_wait() will return -1
 *
 * @param poller    the poller
 */
xu_void_t xu_poller_kill(xu_poller_ref_t poller);

/*! spank the poller, break the xu_poller_wait() and return all events
 *
 * @param poller    the poller
 */
xu_void_t xu_poller_spak(xu_poller_ref_t poller);

/*! the events(clear, oneshot, ..) is supported for the poller?
 *
 * @param poller    the poller
 * @param events    the poller events
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_poller_support(xu_poller_ref_t poller, xu_size_t events);

/*! insert object to poller
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events, it will be ignored if be process object
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_poller_insert(xu_poller_ref_t poller, xu_poller_object_ref_t object, xu_size_t events, xu_cpointer_t priv);

/*! remove object from poller
 *
 * @param poller    the poller
 * @param object    the poller object
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_poller_remove(xu_poller_ref_t poller, xu_poller_object_ref_t object);

/*! modify events for the given poller object
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events, it will be ignored if be process object
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_poller_modify(xu_poller_ref_t poller, xu_poller_object_ref_t object, xu_size_t events, xu_cpointer_t priv);

/*! wait all sockets
 *
 * @param poller    the poller
 * @param func      the events function
 * @param timeout   the timeout, infinity: -1
 *
 * @return          > 0: the events number, 0: timeout or interrupted, -1: failed
 */
xu_long_t xu_poller_wait(xu_poller_ref_t poller, xu_poller_event_func_t func, xu_long_t timeout);

/* //////////////////////////////////////////////////////////
 * inline implementation
 */

/*! insert socket to poller
 *
 * @param poller    the poller
 * @param sock      the socket
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_insert_sock(xu_poller_ref_t poller, xu_socket_ref_t sock, xu_size_t events,
                                                     xu_cpointer_t priv)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return xu_poller_insert(poller, &object, events, priv);
}

/*! remove socket from poller
 *
 * @param poller    the poller
 * @param sock      the socket
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_remove_sock(xu_poller_ref_t poller, xu_socket_ref_t sock)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return xu_poller_remove(poller, &object);
}

/*! modify events for the given socket
 *
 * @param poller    the poller
 * @param sock      the socket
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_modify_sock(xu_poller_ref_t poller, xu_socket_ref_t sock, xu_size_t events,
                                                     xu_cpointer_t priv)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return xu_poller_modify(poller, &object, events, priv);
}

/*! insert pipe to poller
 *
 * @param poller    the poller
 * @param pipe      the pipe
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_insert_pipe(xu_poller_ref_t poller, xu_pipe_file_ref_t pipe, xu_size_t events,
                                                     xu_cpointer_t priv)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return xu_poller_insert(poller, &object, events, priv);
}

/*! remove pipe from poller
 *
 * @param poller    the poller
 * @param pipe      the pipe
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_remove_pipe(xu_poller_ref_t poller, xu_pipe_file_ref_t pipe)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return xu_poller_remove(poller, &object);
}

/*! modify events for the given pipe
 *
 * @param poller    the poller
 * @param pipe      the pipe
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          xu_true or xu_false
 */
static __xu_inline__ xu_bool_t xu_poller_modify_pipe(xu_poller_ref_t poller, xu_pipe_file_ref_t pipe, xu_size_t events,
                                                     xu_cpointer_t priv)
{
    xu_poller_object_t object;
    object.type     = XU_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return xu_poller_modify(poller, &object, events, priv);
}
