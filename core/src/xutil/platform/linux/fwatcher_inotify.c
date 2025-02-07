#include "xutil/algorithm/find_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/container/queue.h"
#include "xutil/libc/libc.h"
#include "xutil/platform/directory.h"
#include "xutil/platform/file.h"
#include "xutil/platform/fwatcher.h"
#include "xutil/platform/impl/pollerdata.h"
#include "xutil/platform/poller.h"
#include "xutil/platform/socket.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

/* *******************************************************
 * macros
 */
#define XU_FWATCHER_EVENT_SIZE (sizeof(struct inotify_event))
#ifdef XU_CONFIG_SMALL
#    define XU_FWATCHER_BUFFER_SIZE (4096 * (XU_FWATCHER_EVENT_SIZE + 16))
#else
#    define XU_FWATCHER_BUFFER_SIZE (8192 * (XU_FWATCHER_EVENT_SIZE + 16))
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
    xu_int_t          fd;
    xu_byte_t         buffer[XU_FWATCHER_BUFFER_SIZE];
    xu_poller_ref_t   poller;
    xu_hash_map_ref_t watchitems;
    xu_queue_ref_t    waited_events;

} xu_fwatcher_t;

/* *******************************************************
 * private implementation
 */
static xu_void_t xu_fwatcher_event(xu_poller_ref_t poller, xu_poller_object_ref_t object, xu_long_t events,
                                   xu_cpointer_t priv)
{
    // we need only an empty callback
}

static xu_bool_t xu_fwatcher_item_eq(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    xu_hash_map_item_ref_t hashitem  = (xu_hash_map_item_ref_t)item;
    xu_fwatcher_item_t*    watchitem = (xu_fwatcher_item_t*)hashitem->data;
    if (watchitem && watchitem->watchdir && value && !xu_strcmp(watchitem->watchdir, value)) return xu_true;
    return xu_false;
}

static xu_void_t xu_fwatcher_item_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_fwatcher_t*      fwatcher  = (xu_fwatcher_t*)element->priv;
    xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)buff;
    xu_assert_and_check_return(fwatcher && watchitem);

    if (fwatcher->fd >= 0 && watchitem->wd >= 0)
    {
        inotify_rm_watch(fwatcher->fd, watchitem->wd);
        watchitem->wd = -1;
    }

    if (watchitem->watchdir) xu_free(watchitem->watchdir);
    watchitem->watchdir = xu_null;
}

static xu_bool_t xu_fwatcher_add_watch(xu_fwatcher_t* fwatcher, xu_char_t const* watchdir, xu_bool_t recursion)
{
    // check
    xu_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, xu_false);

    // this directory has been watched?
    xu_size_t itor = xu_find_all_if(fwatcher->watchitems, xu_fwatcher_item_eq, watchdir);
    if (itor != xu_iterator_tail(fwatcher->watchitems)) return xu_true;

    // add watch
    xu_int_t wd = inotify_add_watch(fwatcher->fd, watchdir, IN_MODIFY | IN_CREATE | IN_DELETE);
    xu_assert_and_check_return_val(wd >= 0, xu_false);

    // save wd -> watchitem
    xu_fwatcher_item_t watchitem;
    watchitem.wd        = wd;
    watchitem.watchdir  = xu_strdup(watchdir);
    watchitem.recursion = recursion;
    xu_hash_map_insert(fwatcher->watchitems, xu_i2p(wd), &watchitem);
    return xu_true;
}

static xu_long_t xu_fwatcher_add_watch_dirs(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)priv;
    xu_assert_and_check_return_val(path && info && fwatcher, XU_DIRECTORY_WALK_CODE_END);

    // add watch directory
    if (info->type == XU_FILE_TYPE_DIRECTORY) xu_fwatcher_add_watch(fwatcher, path, xu_true);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
}

static xu_bool_t xu_fwatcher_rm_watch(xu_fwatcher_t* fwatcher, xu_char_t const* watchdir)
{
    // check
    xu_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, xu_false);

    // remove watchitem
    xu_remove_first_if(fwatcher->watchitems, xu_fwatcher_item_eq, watchdir);
    return xu_true;
}

static xu_long_t xu_fwatcher_rm_watch_dirs(xu_char_t const* path, xu_file_info_t const* info, xu_cpointer_t priv)
{
    // check
    xu_value_t* values = (xu_value_t*)priv;
    xu_assert_and_check_return_val(path && info && values, XU_DIRECTORY_WALK_CODE_END);

    // get fwatcher
    xu_fwatcher_t* fwatcher = values[0].ptr;
    xu_assert_and_check_return_val(fwatcher, XU_DIRECTORY_WALK_CODE_END);

    // rm watch directory
    if (info->type == XU_FILE_TYPE_DIRECTORY) xu_fwatcher_rm_watch(fwatcher, path);
    return XU_DIRECTORY_WALK_CODE_CONTINUE;
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

        // init inotify
        fwatcher->fd = inotify_init();
        xu_assert_and_check_break(fwatcher->fd >= 0);

        // init poller
        fwatcher->poller = xu_poller_init(xu_null);
        xu_assert_and_check_break(fwatcher->poller);

        xu_poller_insert_sock(fwatcher->poller, xu_fd2sock(fwatcher->fd), XU_POLLER_EVENT_RECV, xu_null);

        // init watchitems
        fwatcher->watchitems = xu_hash_map_init(
            0, xu_element_uint32(), xu_element_mem(sizeof(xu_fwatcher_item_t), xu_fwatcher_item_free, fwatcher));
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
        // exit poller
        if (fwatcher->poller) xu_poller_exit(fwatcher->poller);

        // exit watchitems
        if (fwatcher->watchitems)
        {
            xu_hash_map_exit(fwatcher->watchitems);
            fwatcher->watchitems = xu_null;
        }

        // exit fd
        if (fwatcher->fd >= 0)
        {
            close(fwatcher->fd);
            fwatcher->fd = -1;
        }

        // exit waited events
        if (fwatcher->waited_events)
        {
            xu_queue_exit(fwatcher->waited_events);
            fwatcher->waited_events = xu_null;
        }

        // exit watcher
        xu_free(fwatcher);
        fwatcher = xu_null;
    }
}

xu_bool_t xu_fwatcher_add(xu_fwatcher_ref_t self, xu_char_t const* watchdir, xu_bool_t recursion)
{
    // check
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->watchitems && watchdir, xu_false);

    // directory not found
    xu_file_info_t info;
    if (!xu_file_info(watchdir, &info) || info.type != XU_FILE_TYPE_DIRECTORY) return xu_false;

    // is directory? we need scan it and add all subdirs
    if (info.type == XU_FILE_TYPE_DIRECTORY && recursion)
        xu_directory_walk(watchdir, 0, xu_true, xu_fwatcher_add_watch_dirs, fwatcher);
    return xu_fwatcher_add_watch(fwatcher, watchdir, recursion);
}

xu_bool_t xu_fwatcher_remove(xu_fwatcher_ref_t self, xu_char_t const* watchdir)
{
    // check
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && watchdir, xu_false);

    // is directory? we need scan it and remove all subdirs
    xu_file_info_t info;
    if (xu_file_info(watchdir, &info) && info.type == XU_FILE_TYPE_DIRECTORY)
    {
        xu_value_t values[1];
        values[0].ptr = (xu_pointer_t)fwatcher;
        xu_directory_walk(watchdir, 0, xu_false, xu_fwatcher_rm_watch_dirs, values);
    }
    return xu_fwatcher_rm_watch(fwatcher, watchdir);
}

xu_void_t xu_fwatcher_spak(xu_fwatcher_ref_t self)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    if (fwatcher && fwatcher->poller) xu_poller_spak(fwatcher->poller);
}

xu_long_t xu_fwatcher_wait(xu_fwatcher_ref_t self, xu_fwatcher_event_t* event, xu_long_t timeout)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->fd >= 0 && fwatcher->waited_events && event, -1);

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

    // wait events
    xu_long_t wait = xu_poller_wait(fwatcher->poller, xu_fwatcher_event, timeout);
    xu_assert_and_check_return_val(wait >= 0, -1);
    xu_check_return_val(wait > 0, 0);

    xu_int_t real = read(fwatcher->fd, fwatcher->buffer, sizeof(fwatcher->buffer));
    xu_check_return_val(real >= 0, -1);

    xu_int_t i = 0;
    while (i < real)
    {
        struct inotify_event* ievent = (struct inotify_event*)&fwatcher->buffer[i];
        i += XU_FWATCHER_EVENT_SIZE + ievent->len;

        // get event code
        xu_size_t event_code = 0;
        if (ievent->mask & IN_CREATE)
            event_code = XU_FWATCHER_EVENT_CREATE;
        else if (ievent->mask & IN_DELETE)
            event_code = XU_FWATCHER_EVENT_DELETE;
        else if (ievent->mask & IN_MODIFY)
            event_code = XU_FWATCHER_EVENT_MODIFY;

        // get watchitem
        xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)xu_hash_map_get(fwatcher->watchitems, xu_i2p(ievent->wd));
        xu_check_continue(watchitem);
        xu_assert_and_check_break(watchitem->watchdir);

        // add event
        if (event_code)
        {
            xu_fwatcher_event_t evt;
            if (ievent->len)
                xu_snprintf(evt.filepath, XU_PATH_MAXN, "%s/%s", watchitem->watchdir, ievent->name);
            else
                xu_strlcpy(evt.filepath, watchitem->watchdir, XU_PATH_MAXN);
            evt.event = event_code;
            xu_queue_put(fwatcher->waited_events, &evt);

            // rescan the watch directory
            xu_file_info_t info;
            if (watchitem->recursion && xu_file_info(evt.filepath, &info) && info.type == XU_FILE_TYPE_DIRECTORY)
            {
                if (event_code == XU_FWATCHER_EVENT_MODIFY || event_code == XU_FWATCHER_EVENT_CREATE)
                    xu_fwatcher_add(self, evt.filepath, watchitem->recursion);
                else if (event_code == XU_FWATCHER_EVENT_DELETE)
                    xu_fwatcher_remove(self, evt.filepath);
            }
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
