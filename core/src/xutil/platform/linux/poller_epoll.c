#include "xutil/macros/malloc.h"
#include "xutil/platform/impl/poller.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#ifdef XU_CONFIG_POSIX_HAVE_GETRLIMIT
#    include <sys/resource.h>
#endif

/* *******************************************************
 * types
 */

// the epoll poller type
typedef struct __xu_poller_epoll_t
{
    // the poller base
    xu_poller_t base;

    // the maxn
    xu_size_t maxn;

    // the pair sockets for spak, kill ..
    xu_socket_ref_t pair[2];

    // the epoll fd
    xu_long_t epfd;

    // the events
    struct epoll_event* events;

    // the events count
    xu_size_t events_count;

    // the socket data
    xu_pollerdata_t pollerdata;

} xu_poller_epoll_t, *xu_poller_epoll_ref_t;

/* *******************************************************
 * private implementation
 */
static xu_size_t xu_poller_epoll_maxfds()
{
    // attempt to get it from getdtablesize
    xu_size_t maxfds = 0;
#ifdef XU_CONFIG_POSIX_HAVE_GETDTABLESIZE
    if (!maxfds) maxfds = getdtablesize();
#endif

        // attempt to get it from getrlimit
#ifdef XU_CONFIG_POSIX_HAVE_GETRLIMIT
    struct rlimit rl;
    if (!maxfds && !getrlimit(RLIMIT_NOFILE, &rl)) maxfds = rl.rlim_cur;
#endif

    // attempt to get it from sysconf
    if (!maxfds) maxfds = sysconf(_SC_OPEN_MAX);

    // ok?
    return maxfds;
}
static xu_void_t xu_poller_epoll_exit(xu_poller_t* self)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) xu_socket_exit(poller->pair[0]);
    if (poller->pair[1]) xu_socket_exit(poller->pair[1]);
    poller->pair[0] = xu_null;
    poller->pair[1] = xu_null;

    // exit events
    if (poller->events) xu_free(poller->events);
    poller->events       = xu_null;
    poller->events_count = 0;

    // close epfd
    if (poller->epfd > 0) close(poller->epfd);
    poller->epfd = 0;

    // exit socket data
    xu_pollerdata_exit(&poller->pollerdata);

    // free it
    xu_free(poller);
}
static xu_void_t xu_poller_epoll_kill(xu_poller_t* self)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) xu_socket_send(poller->pair[0], (xu_byte_t const*)"k", 1);
}
static xu_void_t xu_poller_epoll_spak(xu_poller_t* self)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) xu_socket_send(poller->pair[0], (xu_byte_t const*)"p", 1);
}
static xu_bool_t xu_poller_epoll_insert(xu_poller_t* self, xu_poller_object_ref_t object, xu_size_t events,
                                        xu_cpointer_t priv)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->epfd > 0 && object, xu_false);

    // init event
    struct epoll_event e = {0};
    if (events & XU_POLLER_EVENT_RECV) e.events |= EPOLLIN;
    if (events & XU_POLLER_EVENT_SEND) e.events |= EPOLLOUT;
    if (events & XU_POLLER_EVENT_CLEAR)
    {
        e.events |= EPOLLET;
#ifdef EPOLLRDHUP
        e.events |= EPOLLRDHUP;
#endif
    }
#ifdef EPOLLONESHOT
    if (events & XU_POLLER_EVENT_ONESHOT) e.events |= EPOLLONESHOT;
#else
    // oneshot is not supported now
    xu_assertf(!(events & XU_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");
#endif

    // save fd
    e.data.fd = (xu_int_t)xu_ptr2fd(object->ref.ptr);

    // bind the object type to the private data
    priv = xu_poller_priv_set_object_type(object, priv);

    // bind user private data to object
    if (!(events & XU_POLLER_EVENT_NOEXTRA) || object->type == XU_POLLER_OBJECT_PIPE)
        xu_pollerdata_set(&poller->pollerdata, object, priv);

    // add socket and events
    if (epoll_ctl(poller->epfd, EPOLL_CTL_ADD, e.data.fd, &e) < 0)
    {
        // trace
        xu_trace_e("insert object(%p) events: %lu failed, errno: %d", object->ref.ptr, events, errno);
        return xu_false;
    }

    // ok
    return xu_true;
}
static xu_bool_t xu_poller_epoll_remove(xu_poller_t* self, xu_poller_object_ref_t object)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->epfd > 0 && object, xu_false);

    // remove object and events
    struct epoll_event e  = {0};
    xu_long_t          fd = xu_ptr2fd(object->ref.ptr);
    if (epoll_ctl(poller->epfd, EPOLL_CTL_DEL, fd, &e) < 0)
    {
        // trace
        xu_trace_e("remove object(%p) failed, errno: %d", object->ref.ptr, errno);
        return xu_false;
    }

    // remove user private data from this object
    xu_pollerdata_reset(&poller->pollerdata, object);
    return xu_true;
}
static xu_bool_t xu_poller_epoll_modify(xu_poller_t* self, xu_poller_object_ref_t object, xu_size_t events,
                                        xu_cpointer_t priv)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->epfd > 0 && object, xu_false);

    // init event
    struct epoll_event e = {0};
    if (events & XU_POLLER_EVENT_RECV) e.events |= EPOLLIN;
    if (events & XU_POLLER_EVENT_SEND) e.events |= EPOLLOUT;
    if (events & XU_POLLER_EVENT_CLEAR)
    {
        e.events |= EPOLLET;
#ifdef EPOLLRDHUP
        e.events |= EPOLLRDHUP;
#endif
    }
#ifdef EPOLLONESHOT
    if (events & XU_POLLER_EVENT_ONESHOT) e.events |= EPOLLONESHOT;
#else
    // oneshot is not supported now
    xu_assertf(!(events & XU_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");
#endif

    // save fd
    e.data.fd = (xu_int_t)xu_ptr2fd(object->ref.ptr);

    // bind the object type to the private data
    priv = xu_poller_priv_set_object_type(object, priv);

    // bind user private data to object
    if (!(events & XU_POLLER_EVENT_NOEXTRA) || object->type == XU_POLLER_OBJECT_PIPE)
        xu_pollerdata_set(&poller->pollerdata, object, priv);

    // modify events
    if (epoll_ctl(poller->epfd, EPOLL_CTL_MOD, e.data.fd, &e) < 0)
    {
        // trace
        xu_trace_e("modify object(%p) events: %lu failed, errno: %d", object->ref.ptr, events, errno);
        return xu_false;
    }
    return xu_true;
}
static xu_long_t xu_poller_epoll_wait(xu_poller_t* self, xu_poller_event_func_t func, xu_long_t timeout)
{
    // check
    xu_poller_epoll_ref_t poller = (xu_poller_epoll_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->epfd > 0 && poller->maxn && func, -1);

    // init events
    xu_size_t grow = xu_align8((poller->maxn >> 3) + 1);
    if (!poller->events)
    {
        poller->events_count = grow;
        poller->events       = xu_nalloc_type(poller->events_count, struct epoll_event);
        xu_assert_and_check_return_val(poller->events, -1);
    }

    // wait events
    xu_long_t events_count = epoll_wait(poller->epfd, poller->events, poller->events_count, timeout);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR)) return 0;

    // check error?
    xu_assert_and_check_return_val(events_count >= 0 && events_count <= poller->events_count, -1);

    // grow it if events is full
    if (events_count == poller->events_count)
    {
        // grow size
        poller->events_count += grow;
        if (poller->events_count > poller->maxn) poller->events_count = poller->maxn;

        // grow data
        poller->events =
            (struct epoll_event*)xu_ralloc(poller->events, poller->events_count * sizeof(struct epoll_event));
        xu_assert_and_check_return_val(poller->events, -1);
    }
    xu_assert(events_count <= poller->events_count);

    // limit
    events_count = xu_min(events_count, poller->maxn);

    // handle events
    xu_size_t           i    = 0;
    xu_size_t           wait = 0;
    struct epoll_event* e    = xu_null;
    xu_socket_ref_t     pair = poller->pair[1];
    xu_poller_object_t  object;
    for (i = 0; i < events_count; i++)
    {
        // the epoll event
        e = poller->events + i;

        // the events for epoll
        xu_size_t epoll_events = e->events;

        // the socket
        xu_long_t fd   = e->data.fd;
        object.ref.ptr = xu_fd2ptr(fd);
        xu_assert(object.ref.ptr);

        // spank socket events?
        if (object.ref.sock == pair && (epoll_events & EPOLLIN))
        {
            // read spak
            xu_char_t spak = '\0';
            if (1 != xu_socket_recv(pair, (xu_byte_t*)&spak, 1)) return -1;

            // killed?
            if (spak == 'k') return -1;

            // continue it
            continue;
        }
        xu_check_continue(object.ref.sock != pair);

        // init events
        xu_size_t events = XU_POLLER_EVENT_NONE;
        if (epoll_events & EPOLLIN) events |= XU_POLLER_EVENT_RECV;
        if (epoll_events & EPOLLOUT) events |= XU_POLLER_EVENT_SEND;
        if (epoll_events & (EPOLLHUP | EPOLLERR) && !(events & (XU_POLLER_EVENT_RECV | XU_POLLER_EVENT_SEND)))
            events |= XU_POLLER_EVENT_RECV | XU_POLLER_EVENT_SEND;

#ifdef EPOLLRDHUP
        // connection closed for the edge trigger?
        if (epoll_events & EPOLLRDHUP) events |= XU_POLLER_EVENT_EOF;
#endif

        // call event function
        xu_cpointer_t priv = xu_pollerdata_get(&poller->pollerdata, &object);
        object.type        = xu_poller_priv_get_object_type(priv);
        func((xu_poller_ref_t)self, &object, events, xu_poller_priv_get_original(priv));

        // update the events count
        wait++;
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_poller_t* xu_poller_epoll_init()
{
    // done
    xu_bool_t             ok     = xu_false;
    xu_poller_epoll_ref_t poller = xu_null;
    do
    {
        // make poller
        poller = xu_malloc0_type(xu_poller_epoll_t);
        xu_assert_and_check_break(poller);

        // init base
        poller->base.type   = XU_POLLER_TYPE_EPOLL;
        poller->base.exit   = xu_poller_epoll_exit;
        poller->base.kill   = xu_poller_epoll_kill;
        poller->base.spak   = xu_poller_epoll_spak;
        poller->base.wait   = xu_poller_epoll_wait;
        poller->base.insert = xu_poller_epoll_insert;
        poller->base.remove = xu_poller_epoll_remove;
        poller->base.modify = xu_poller_epoll_modify;
#ifdef EPOLLONESHOT
        poller->base.supported_events = XU_POLLER_EVENT_EALL | XU_POLLER_EVENT_CLEAR | XU_POLLER_EVENT_ONESHOT;
#else
        poller->base.supported_events = XU_POLLER_EVENT_EALL | XU_POLLER_EVENT_CLEAR;
#endif

        // init poller data
        xu_pollerdata_init(&poller->pollerdata);

        // init maxn
        poller->maxn = xu_poller_epoll_maxfds();
        xu_assert_and_check_break(poller->maxn);

        // init epoll
        poller->epfd = epoll_create(poller->maxn);
        xu_assert_and_check_break(poller->epfd > 0);

        // init pair sockets
        if (!xu_socket_pair(XU_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        xu_poller_object_t object;
        object.type     = XU_POLLER_OBJECT_SOCK;
        object.ref.sock = poller->pair[1];
        if (!xu_poller_epoll_insert((xu_poller_t*)poller, &object, XU_POLLER_EVENT_RECV, xu_null)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) xu_poller_epoll_exit((xu_poller_t*)poller);
        poller = xu_null;
    }

    // ok?
    return (xu_poller_t*)poller;
}
