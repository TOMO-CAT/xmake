#include "xutil/libc/libc.h"
#include "xutil/platform/directory.h"
#include "xutil/platform/file.h"
#include "xutil/platform/fwatcher.h"
#include "xutil/platform/socket.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* *******************************************************
 * macros
 */
#ifndef EV_ENABLE
#    define EV_ENABLE (0)
#endif

#ifndef NOTE_EOF
#    define NOTE_EOF (0)
#endif

/* *******************************************************
 * types
 */

// the watch item type
typedef struct __xu_fwatcher_item_t
{
    xu_int_t         wd;
    xu_char_t const* watchdir;
    xu_bool_t        recursion;

} xu_fwatcher_item_t;

// the fwatcher type
typedef struct __xu_fwatcher_t
{
    xu_int_t          kqfd;
    struct kevent*    watchevents;
    xu_size_t         watchevents_size;
    xu_size_t         watchevents_maxn;
    struct kevent*    events;
    xu_size_t         events_count;
    xu_hash_map_ref_t watchitems;
    xu_socket_ref_t   pair[2];
    xu_queue_ref_t    waited_events;

} xu_fwatcher_t;

/* *******************************************************
 * private implementation
 */
static xu_void_t xu_fwatcher_item_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)buff;
    if (watchitem && watchitem->wd >= 0)
    {
        close(watchitem->wd);
        watchitem->wd = -1;
    }
}

static xu_bool_t xu_fwatcher_add_watch(xu_fwatcher_t* fwatcher, xu_char_t const* watchdir, xu_bool_t recursion)
{
    // check
    xu_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->watchitems && watchdir, xu_false);

    // this path has been added?
    xu_size_t itor = xu_hash_map_find(fwatcher->watchitems, watchdir);
    if (itor != xu_iterator_tail(fwatcher->watchitems)) return xu_true;

    // open watch fd
    xu_int_t o_flags = 0;
#ifdef O_EVTONLY
    o_flags |= O_EVTONLY;
#else
    o_flags |= O_RDONLY;
#endif
    xu_int_t wd = open(watchdir, o_flags);
    xu_check_return_val(wd >= 0, xu_false);

    // save watch item
    xu_fwatcher_item_t watchitem;
    watchitem.wd        = wd;
    watchitem.recursion = recursion;
    watchitem.watchdir  = xu_null;
    return xu_hash_map_insert(fwatcher->watchitems, watchdir, &watchitem) != xu_iterator_tail(fwatcher->watchitems);
}

static xu_long_t xu_fwatcher_add_watch_filedirs(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)priv;
    xu_assert_and_check_return_val(path && info && fwatcher, XU_DIRECTORY_WALK_CODE_END);

    // add file watch
    xu_fwatcher_add_watch(fwatcher, path, xu_true);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
}

static xu_bool_t xu_fwatcher_rm_watch(xu_fwatcher_t* fwatcher, xu_char_t const* watchdir)
{
    // check
    xu_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->watchitems && watchdir, xu_false);

    // remove the watchitem
    xu_hash_map_remove(fwatcher->watchitems, watchdir);
    return xu_true;
}

static xu_long_t xu_fwatcher_rm_watch_filedirs(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_value_t* values = (xu_value_t*)priv;
    xu_assert_and_check_return_val(path && info && values, XU_DIRECTORY_WALK_CODE_END);

    // get fwatcher
    xu_fwatcher_t* fwatcher = values[0].ptr;
    xu_assert_and_check_return_val(fwatcher, XU_DIRECTORY_WALK_CODE_END);

    // rm file watch
    xu_fwatcher_rm_watch(fwatcher, path);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
}

static xu_bool_t xu_fwatcher_update_watchevents(xu_iterator_ref_t iterator, xu_pointer_t item, xu_cpointer_t priv)
{
    // check
    xu_fwatcher_t*         fwatcher = (xu_fwatcher_t*)priv;
    xu_hash_map_item_ref_t hashitem = (xu_hash_map_item_ref_t)item;
    xu_assert_and_check_return_val(fwatcher && fwatcher->watchitems && hashitem, xu_false);

    // get watch item and path
    xu_char_t const*    path      = (xu_char_t const*)hashitem->name;
    xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)hashitem->data;
    xu_assert_and_check_return_val(watchitem->wd >= 0 && path, xu_false);

    // grow watchevents
    xu_size_t watchsize = xu_hash_map_size(fwatcher->watchitems);
    if (!fwatcher->watchevents)
    {
        fwatcher->watchevents_maxn = watchsize;
        fwatcher->watchevents      = xu_nalloc_type(1 + fwatcher->watchevents_maxn, struct kevent);
    }
    else if (fwatcher->watchevents_size >= fwatcher->watchevents_maxn)
    {
        fwatcher->watchevents_maxn = watchsize + 64;
        fwatcher->watchevents =
            xu_ralloc(fwatcher->watchevents, (1 + fwatcher->watchevents_maxn) * sizeof(struct kevent));
    }
    xu_assert_and_check_return_val(fwatcher->watchevents && fwatcher->watchevents_size < fwatcher->watchevents_maxn,
                                   xu_false);

    // register pair1 to watchevents first
    if (!fwatcher->watchevents_size)
    {
        EV_SET(&fwatcher->watchevents[0], xu_sock2fd(fwatcher->pair[1]), EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR,
               NOTE_EOF, 0, xu_null);
    }

    // register watch events
    xu_uint_t vnode_events =
        NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE;
    xu_assert_and_check_return_val(vnode_events, xu_false);

    watchitem->watchdir = path;
    EV_SET(&fwatcher->watchevents[1 + fwatcher->watchevents_size], watchitem->wd, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_CLEAR, vnode_events, 0, (xu_pointer_t)watchitem);
    fwatcher->watchevents_size++;
    return xu_true;
}

/* *******************************************************
 * implementation
 */
xu_fwatcher_ref_t xu_fwatcher_init()
{
    xu_bool_t      ok       = xu_false;
    xu_fwatcher_t* fwatcher = xu_null;
    do
    {
        // init fwatcher
        fwatcher = xu_malloc0_type(xu_fwatcher_t);
        xu_assert_and_check_break(fwatcher);

        // init kqueue
        fwatcher->kqfd = kqueue();
        xu_assert_and_check_break(fwatcher->kqfd >= 0);

        // init socket pair
        if (!xu_socket_pair(XU_SOCKET_TYPE_TCP, fwatcher->pair)) break;

        // init watch items
        fwatcher->watchitems = xu_hash_map_init(
            0, xu_element_str(xu_true), xu_element_mem(sizeof(xu_fwatcher_item_t), xu_fwatcher_item_free, xu_null));
        xu_assert_and_check_break(fwatcher->watchitems);

        // init waited events
        fwatcher->waited_events = xu_queue_init(0, xu_element_mem(sizeof(xu_fwatcher_event_t), xu_null, xu_null));
        xu_assert_and_check_break(fwatcher->waited_events);

        ok = xu_true;
    } while (0);

    if (!ok && fwatcher)
    {
        xu_fwatcher_exit((xu_fwatcher_ref_t)fwatcher);
        fwatcher = xu_null;
    }
    return (xu_fwatcher_ref_t)fwatcher;
}

xu_void_t xu_fwatcher_exit(xu_fwatcher_ref_t self)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    if (fwatcher)
    {
        // exit pair sockets
        if (fwatcher->pair[0]) xu_socket_exit(fwatcher->pair[0]);
        if (fwatcher->pair[1]) xu_socket_exit(fwatcher->pair[1]);
        fwatcher->pair[0] = xu_null;
        fwatcher->pair[1] = xu_null;

        // exit events
        if (fwatcher->events) xu_free(fwatcher->events);
        fwatcher->events       = xu_null;
        fwatcher->events_count = 0;

        // exit watch events
        if (fwatcher->watchevents) xu_free(fwatcher->watchevents);
        fwatcher->watchevents      = xu_null;
        fwatcher->watchevents_size = 0;
        fwatcher->watchevents_maxn = 0;

        // exit watch items
        if (fwatcher->watchitems)
        {
            xu_hash_map_exit(fwatcher->watchitems);
            fwatcher->watchitems = xu_null;
        }

        // exit waited events
        if (fwatcher->waited_events)
        {
            xu_queue_exit(fwatcher->waited_events);
            fwatcher->waited_events = xu_null;
        }

        // exit kqueue fd
        if (fwatcher->kqfd >= 0)
        {
            close(fwatcher->kqfd);
            fwatcher->kqfd = -1;
        }

        // wait watcher
        xu_free(fwatcher);
        fwatcher = xu_null;
    }
}

xu_bool_t xu_fwatcher_add(xu_fwatcher_ref_t self, xu_char_t const* watchdir, xu_bool_t recursion)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && watchdir, xu_false);

    // file not found
    xu_file_info_t info;
    if (!xu_file_info(watchdir, &info) || info.type != XU_FILE_TYPE_DIRECTORY) return xu_false;

    // is directory? we need scan it and add all subfiles
    if (info.type == XU_FILE_TYPE_DIRECTORY && recursion)
        xu_directory_walk(watchdir, -1, xu_true, xu_fwatcher_add_watch_filedirs, fwatcher);
    return xu_fwatcher_add_watch(fwatcher, watchdir, recursion);
}

xu_bool_t xu_fwatcher_remove(xu_fwatcher_ref_t self, xu_char_t const* watchdir)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && watchdir, xu_false);

    // is directory? we need scan it and remove all subfiles
    xu_file_info_t info;
    if (xu_file_info(watchdir, &info) && info.type == XU_FILE_TYPE_DIRECTORY)
    {
        xu_value_t values[1];
        values[0].ptr = (xu_pointer_t)fwatcher;
        xu_directory_walk(watchdir, 0, xu_false, xu_fwatcher_rm_watch_filedirs, values);
    }
    return xu_fwatcher_rm_watch(fwatcher, watchdir);
}

xu_void_t xu_fwatcher_spak(xu_fwatcher_ref_t self)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return(fwatcher);

    if (fwatcher->pair[0]) xu_socket_send(fwatcher->pair[0], (xu_byte_t const*)"p", 1);
}

xu_long_t xu_fwatcher_wait(xu_fwatcher_ref_t self, xu_fwatcher_event_t* event, xu_long_t timeout)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->kqfd >= 0 && fwatcher->waited_events && event, -1);

#if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && xu_coroutine_self())
    {
        xu_poller_object_t object;
        object.type         = XU_POLLER_OBJECT_FWATCHER;
        object.ref.fwatcher = self;
        return xu_coroutine_waitfs(&object, event, timeout);
    }
#endif

    // get it if has events
    xu_bool_t has_events = xu_false;
    if (!xu_queue_null(fwatcher->waited_events))
    {
        xu_fwatcher_event_t* e = (xu_fwatcher_event_t*)xu_queue_get(fwatcher->waited_events);
        if (e)
        {
            *event = *e;
            xu_queue_pop(fwatcher->waited_events);
            has_events = xu_true;
        }
    }
    xu_check_return_val(!has_events, 1);

    // init time
    struct timespec t = {0};
    if (timeout > 0)
    {
        t.tv_sec  = timeout / 1000;
        t.tv_nsec = (timeout % 1000) * 1000000;
    }

    // init events
    xu_size_t grow = 256;
    if (!fwatcher->events)
    {
        fwatcher->events_count = grow;
        fwatcher->events       = xu_nalloc_type(fwatcher->events_count, struct kevent);
        xu_assert_and_check_return_val(fwatcher->events, -1);
    }

    // update watch events
    fwatcher->watchevents_size = 0;
    xu_walk_all(fwatcher->watchitems, xu_fwatcher_update_watchevents, fwatcher);
    xu_assert_and_check_return_val(fwatcher->watchevents && fwatcher->watchevents_size, -1);

    // wait events
    xu_long_t events_count = kevent(fwatcher->kqfd, fwatcher->watchevents, fwatcher->watchevents_size, fwatcher->events,
                                    fwatcher->events_count, timeout >= 0 ? &t : xu_null);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR)) return 0;

    // error?
    xu_assert_and_check_return_val(events_count >= 0 && events_count <= fwatcher->events_count, -1);

    // grow it if events is full
    if (events_count == fwatcher->events_count)
    {
        // grow size
        fwatcher->events_count += grow;

        // grow data
        fwatcher->events = (struct kevent*)xu_ralloc(fwatcher->events, fwatcher->events_count * sizeof(struct kevent));
        xu_assert_and_check_return_val(fwatcher->events, -1);
    }
    xu_assert(events_count <= fwatcher->events_count);

    // handle events
    xu_size_t       i    = 0;
    xu_socket_ref_t pair = fwatcher->pair[1];
    struct kevent*  kevt = xu_null;
    for (i = 0; i < events_count; i++)
    {
        // get event
        kevt = fwatcher->events + i;
        if (kevt->flags & EV_ERROR) continue;

        // spank socket events?
        xu_socket_ref_t sock = xu_fd2sock(kevt->ident);
        if (sock == pair && kevt->filter == EVFILT_READ)
        {
            xu_char_t spak = '\0';
            if (1 != xu_socket_recv(pair, (xu_byte_t*)&spak, 1)) return -1;
            continue;
        }

        // get watchitem
        xu_fwatcher_item_t const* watchitem = (xu_fwatcher_item_t const*)kevt->udata;
        xu_assert_and_check_break(watchitem);

        // get event code
        xu_size_t event_code = 0;
        if (kevt->fflags & NOTE_DELETE)
            event_code = XU_FWATCHER_EVENT_DELETE;
        else if ((kevt->fflags & NOTE_RENAME) || (kevt->fflags & NOTE_REVOKE) || (kevt->fflags & NOTE_WRITE))
            event_code = XU_FWATCHER_EVENT_MODIFY;

        // add event
        if (event_code)
        {
            xu_fwatcher_event_t evt;
            if (watchitem->watchdir)
                xu_strlcpy(evt.filepath, watchitem->watchdir, XU_PATH_MAXN);
            else
                evt.filepath[0] = '\0';
            evt.event = event_code;
            xu_queue_put(fwatcher->waited_events, &evt);
        }

        // rescan the watch directory
        if (watchitem->watchdir)
        {
            xu_file_info_t info;
            if ((event_code == XU_FWATCHER_EVENT_MODIFY || event_code == XU_FWATCHER_EVENT_CREATE) &&
                xu_file_info(watchitem->watchdir, &info) && info.type == XU_FILE_TYPE_DIRECTORY)
                xu_fwatcher_add(self, watchitem->watchdir, watchitem->recursion);
            else if (event_code == XU_FWATCHER_EVENT_DELETE)
                xu_fwatcher_remove(self, watchitem->watchdir);
        }
    }

    // get event
    if (!xu_queue_null(fwatcher->waited_events))
    {
        xu_fwatcher_event_t* e = (xu_fwatcher_event_t*)xu_queue_get(fwatcher->waited_events);
        if (e)
        {
            *event = *e;
            xu_queue_pop(fwatcher->waited_events);
            has_events = xu_true;
        }
    }
    return has_events ? 1 : 0;
}
