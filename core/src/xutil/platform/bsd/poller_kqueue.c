#pragma once

#include "xutil/macros/malloc.h"
#include "xutil/platform/impl/poller.h"

#if defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_BSD)
#    include <errno.h>
#    include <stdint.h>
#    include <sys/event.h>
#    include <sys/time.h>
#    include <sys/types.h>
#    include <unistd.h>
#    ifdef XU_CONFIG_POSIX_HAVE_GETRLIMIT
#        include <sys/resource.h>
#    endif

/* *******************************************************
 * macros
 */
#    ifndef EV_ENABLE
#        define EV_ENABLE (0)
#    endif

#    ifndef NOTE_EOF
#        define NOTE_EOF (0)
#    endif

/* *******************************************************
 * types
 */

// the kqueue poller type
typedef struct __xu_poller_kqueue_t
{
    // the poller base
    xu_poller_t base;

    // the maxn
    xu_size_t maxn;

    // the pair sockets for spak, kill ..
    xu_socket_ref_t pair[2];

    // the kqueue fd
    xu_long_t kqfd;

    // the events
    struct kevent* events;

    // the events count
    xu_size_t events_count;

    // the poller data
    xu_pollerdata_t pollerdata;

} xu_poller_kqueue_t, *xu_poller_kqueue_ref_t;

/* *******************************************************
 * private implementation
 */

static xu_size_t xu_poller_kqueue_maxfds()
{
    // attempt to get it from getdtablesize
    xu_size_t maxfds = 0;
#    ifdef XU_CONFIG_POSIX_HAVE_GETDTABLESIZE
    if (!maxfds) maxfds = getdtablesize();
#    endif

        // attempt to get it from getrlimit
#    ifdef XU_CONFIG_POSIX_HAVE_GETRLIMIT
    struct rlimit rl;
    if (!maxfds && !getrlimit(RLIMIT_NOFILE, &rl)) maxfds = rl.rlim_cur;
#    endif

    // attempt to get it from sysconf
    if (!maxfds) maxfds = sysconf(_SC_OPEN_MAX);

    // ok?
    return maxfds;
}
static xu_bool_t xu_poller_kqueue_change(xu_poller_kqueue_ref_t poller, struct kevent* events, xu_size_t count)
{
    // check
    xu_assert_and_check_return_val(events && count, xu_false);

    // change events
    struct timespec t = {0};
    if (kevent(poller->kqfd, events, count, xu_null, 0, &t) < 0)
    {
        // trace
        xu_trace_e("change kevent failed, errno: %d", errno);

        // failed
        return xu_false;
    }

    // ok
    return xu_true;
}
static xu_void_t xu_poller_kqueue_exit(xu_poller_t* self)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
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

    // close kqfd
    if (poller->kqfd > 0) close(poller->kqfd);
    poller->kqfd = 0;

    // exit socket data
    xu_pollerdata_exit(&poller->pollerdata);

    // free it
    xu_free(poller);
}
static xu_void_t xu_poller_kqueue_kill(xu_poller_t* self)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) xu_socket_send(poller->pair[0], (xu_byte_t const*)"k", 1);
}
static xu_void_t xu_poller_kqueue_spak(xu_poller_t* self)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) xu_socket_send(poller->pair[0], (xu_byte_t const*)"p", 1);
}
static xu_bool_t xu_poller_kqueue_insert(xu_poller_t* self, xu_poller_object_ref_t object, xu_size_t events,
                                         xu_cpointer_t priv)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->kqfd > 0 && object, xu_false);

    // init the add event
    xu_size_t adde = EV_ADD | EV_ENABLE;
    if (events & XU_POLLER_EVENT_CLEAR) adde |= EV_CLEAR;
    if (events & XU_POLLER_EVENT_ONESHOT) adde |= EV_ONESHOT;

    // bind the object type to the private data
    priv = xu_poller_priv_set_object_type(object, priv);

    // insert socket and add events
    struct kevent e[2];
    xu_size_t     n  = 0;
    xu_int_t      fd = xu_ptr2fd(object->ref.ptr);
    if (events & XU_POLLER_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, adde, NOTE_EOF, 0, (xu_pointer_t)priv);
        n++;
    }
    if (events & XU_POLLER_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, adde, NOTE_EOF, 0, (xu_pointer_t)priv);
        n++;
    }

    // change it
    xu_bool_t ok = n ? xu_poller_kqueue_change(poller, e, n) : xu_true;

    // save events to object
    if (ok) xu_pollerdata_set(&poller->pollerdata, object, (xu_cpointer_t)events);

    // ok?
    return ok;
}
static xu_bool_t xu_poller_kqueue_remove(xu_poller_t* self, xu_poller_object_ref_t object)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->kqfd > 0 && object, xu_false);

    // get the previous events
    xu_size_t events = (xu_size_t)xu_pollerdata_get(&poller->pollerdata, object);

    // remove this socket and events
    struct kevent e[2];
    xu_size_t     n  = 0;
    xu_int_t      fd = xu_ptr2fd(object->ref.ptr);
    if (events & XU_POLLER_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, xu_null);
        n++;
    }
    if (events & XU_POLLER_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, xu_null);
        n++;
    }

    // change it
    xu_bool_t ok = n ? xu_poller_kqueue_change(poller, e, n) : xu_true;

    // remove events from object
    if (ok) xu_pollerdata_reset(&poller->pollerdata, object);

    // ok?
    return ok;
}
static xu_bool_t xu_poller_kqueue_modify(xu_poller_t* self, xu_poller_object_ref_t object, xu_size_t events,
                                         xu_cpointer_t priv)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->kqfd > 0 && object, xu_false);

    // get the previous events
    xu_size_t events_old = (xu_size_t)xu_pollerdata_get(&poller->pollerdata, object);

    // get changed events
    xu_size_t adde = events & ~events_old;
    xu_size_t dele = ~events & events_old;

    // init the add event
    xu_size_t add_event = EV_ADD | EV_ENABLE;
    if (events & XU_POLLER_EVENT_CLEAR) add_event |= EV_CLEAR;
    if (events & XU_POLLER_EVENT_ONESHOT) add_event |= EV_ONESHOT;

    // bind the object type to the private data
    priv = xu_poller_priv_set_object_type(object, priv);

    // modify events
    struct kevent e[2];
    xu_size_t     n  = 0;
    xu_int_t      fd = xu_ptr2fd(object->ref.ptr);
    if (adde & XU_SOCKET_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, add_event, NOTE_EOF, 0, (xu_pointer_t)priv);
        n++;
    }
    else if (dele & XU_SOCKET_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, (xu_pointer_t)priv);
        n++;
    }
    if (adde & XU_SOCKET_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, add_event, NOTE_EOF, 0, (xu_pointer_t)priv);
        n++;
    }
    else if (dele & XU_SOCKET_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (xu_pointer_t)priv);
        n++;
    }

    // change it
    xu_bool_t ok = n ? xu_poller_kqueue_change(poller, e, n) : xu_true;

    // save events to socket
    if (ok) xu_pollerdata_set(&poller->pollerdata, object, (xu_cpointer_t)events);

    // ok?
    return ok;
}
static xu_long_t xu_poller_kqueue_wait(xu_poller_t* self, xu_poller_event_func_t func, xu_long_t timeout)
{
    // check
    xu_poller_kqueue_ref_t poller = (xu_poller_kqueue_ref_t)self;
    xu_assert_and_check_return_val(poller && poller->kqfd > 0 && poller->maxn && func, -1);

    // init time
    struct timespec t = {0};
    if (timeout > 0)
    {
        t.tv_sec  = timeout / 1000;
        t.tv_nsec = (timeout % 1000) * 1000000;
    }

    // init events
    xu_size_t grow = xu_align8((poller->maxn >> 3) + 1);
    if (!poller->events)
    {
        poller->events_count = grow;
        poller->events       = xu_nalloc_type(poller->events_count, struct kevent);
        xu_assert_and_check_return_val(poller->events, -1);
    }

    // wait events
    xu_long_t events_count =
        kevent(poller->kqfd, xu_null, 0, poller->events, poller->events_count, timeout >= 0 ? &t : xu_null);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR)) return 0;

    // error?
    xu_assert_and_check_return_val(events_count >= 0 && events_count <= poller->events_count, -1);

    // grow it if events is full
    if (events_count == poller->events_count)
    {
        // grow size
        poller->events_count += grow;
        if (poller->events_count > poller->maxn) poller->events_count = poller->maxn;

        // grow data
        poller->events = (struct kevent*)xu_ralloc(poller->events, poller->events_count * sizeof(struct kevent));
        xu_assert_and_check_return_val(poller->events, -1);
    }
    xu_assert(events_count <= poller->events_count);

    // limit
    events_count = xu_min(events_count, poller->maxn);

    // handle events
    xu_size_t          i    = 0;
    xu_size_t          wait = 0;
    struct kevent*     e    = xu_null;
    xu_socket_ref_t    pair = poller->pair[1];
    xu_poller_object_t object;
    for (i = 0; i < events_count; i++)
    {
        // the kevents
        e = poller->events + i;

        // get the object pointer
        object.ref.ptr = xu_fd2ptr(e->ident);
        xu_assert(object.ref.ptr);

        // spank socket events?
        if (object.ref.sock == pair && e->filter == EVFILT_READ)
        {
            // read spak
            xu_char_t spak = '\0';
            if (1 != xu_socket_recv(pair, (xu_byte_t*)&spak, 1)) return -1;

            // killed?
            if (spak == 'k') return -1;

            // continue it
            continue;
        }

        // skip spak
        xu_check_continue(object.ref.sock != pair);

        // init events
        xu_size_t events = XU_POLLER_EVENT_NONE;
        if (e->filter == EVFILT_READ) events |= XU_POLLER_EVENT_RECV;
        if (e->filter == EVFILT_WRITE) events |= XU_POLLER_EVENT_SEND;
        if ((e->flags & EV_ERROR) && !(events & (XU_POLLER_EVENT_RECV | XU_POLLER_EVENT_SEND)))
            events |= XU_POLLER_EVENT_RECV | XU_POLLER_EVENT_SEND;

        // connection closed for the edge trigger?
        if (e->flags & EV_EOF) events |= XU_POLLER_EVENT_EOF;

        // call event function
        object.type = xu_poller_priv_get_object_type(e->udata);
        func((xu_poller_ref_t)self, &object, events, xu_poller_priv_get_original(e->udata));

        // update the events count
        wait++;
    }

    // ok
    return wait;
}

/* *******************************************************
 * implementation
 */

xu_poller_t* xu_poller_kqueue_init()
{
    xu_bool_t              ok     = xu_false;
    xu_poller_kqueue_ref_t poller = xu_null;
    do
    {
        // make poller
        poller = xu_malloc0_type(xu_poller_kqueue_t);
        xu_assert_and_check_break(poller);

        // init base
        poller->base.type             = XU_POLLER_TYPE_KQUEUE;
        poller->base.exit             = xu_poller_kqueue_exit;
        poller->base.kill             = xu_poller_kqueue_kill;
        poller->base.spak             = xu_poller_kqueue_spak;
        poller->base.wait             = xu_poller_kqueue_wait;
        poller->base.insert           = xu_poller_kqueue_insert;
        poller->base.remove           = xu_poller_kqueue_remove;
        poller->base.modify           = xu_poller_kqueue_modify;
        poller->base.supported_events = XU_POLLER_EVENT_EALL | XU_POLLER_EVENT_CLEAR | XU_POLLER_EVENT_ONESHOT;

        // init socket data
        xu_pollerdata_init(&poller->pollerdata);

        // init kqueue
        poller->kqfd = kqueue();
        xu_assert_and_check_break(poller->kqfd > 0);

        // init maxn
        poller->maxn = xu_poller_kqueue_maxfds();
        xu_assert_and_check_break(poller->maxn);

        // init pair sockets
        if (!xu_socket_pair(XU_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        xu_poller_object_t pair1;
        pair1.type     = XU_POLLER_OBJECT_SOCK;
        pair1.ref.sock = poller->pair[1];
        if (!xu_poller_kqueue_insert((xu_poller_t*)poller, &pair1, XU_POLLER_EVENT_RECV, xu_null)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) xu_poller_kqueue_exit((xu_poller_t*)poller);
        poller = xu_null;
    }

    // ok?
    return (xu_poller_t*)poller;
}

#endif // defined(XU_CONFIG_OS_MACOSX) || defined(XU_CONFIG_OS_BSD)
