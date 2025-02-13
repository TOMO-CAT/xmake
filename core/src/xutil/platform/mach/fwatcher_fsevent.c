#if defined(XU_CONFIG_OS_MACOSX)
#    include "xutil/libc/libc.h"
#    include "xutil/platform/directory.h"
#    include "xutil/platform/file.h"
#    include "xutil/platform/fwatcher.h"
#    include "xutil/platform/semaphore.h"
#    include "xutil/platform/socket.h"
#    include "xutil/platform/spinlock.h"
#    include <CoreServices/CoreServices.h>

/* *******************************************************
 * types
 */

// the watch item type
typedef struct __xu_fwatcher_item_t
{
    FSEventStreamContext context;
    FSEventStreamRef     stream;
    dispatch_queue_t     fsevents_queue;
    xu_char_t const*     watchdir;
    xu_bool_t            recursion;
    xu_fwatcher_ref_t    fwatcher;

} xu_fwatcher_item_t;

// the fwatcher type
typedef struct __xu_fwatcher_t
{
    xu_hash_map_ref_t  watchitems;
    xu_semaphore_ref_t semaphore;
    xu_queue_ref_t     events_queue;
    xu_spinlock_t      lock;

} xu_fwatcher_t;

/* *******************************************************
 * private implementation
 */

static xu_void_t xu_fwatcher_item_free(xu_element_ref_t element, xu_pointer_t buff)
{
    xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)buff;
    if (watchitem)
    {
        // exit stream
        if (watchitem->stream)
        {
            FSEventStreamStop(watchitem->stream);
            FSEventStreamInvalidate(watchitem->stream);
            FSEventStreamRelease(watchitem->stream);
            watchitem->stream = xu_null;
        }

        // exit dispatch queue
        if (watchitem->fsevents_queue) dispatch_release(watchitem->fsevents_queue);
        watchitem->fsevents_queue = xu_null;

        // reset watchdir
        watchitem->watchdir = xu_null;
    }
}

static xu_void_t xu_fwatcher_item_callback(ConstFSEventStreamRef stream, xu_pointer_t priv, size_t events_count,
                                           xu_pointer_t event_paths, const FSEventStreamEventFlags event_flags[],
                                           FSEventStreamEventId const* event_id)
{
    // check
    xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)priv;
    xu_assert_and_check_return(watchitem && watchitem->stream && watchitem->watchdir);

    // get fwatcher
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)watchitem->fwatcher;
    xu_assert_and_check_return(fwatcher && fwatcher->semaphore && fwatcher->events_queue);

    // get events
    size_t    i;
    xu_bool_t has_events = xu_false;
    for (i = 0; i < events_count; ++i)
    {
        // get event
#    if defined(HAVE_MACOS_GE_10_13)
        CFDictionaryRef path_info_dict = (CFDictionaryRef)CFArrayGetValueAtIndex((CFArrayRef)event_paths, i);
        CFStringRef path = (CFStringRef)CFDictionaryGetValue(path_info_dict, kFSEventStreamEventExtendedDataPathKey);
        CFNumberRef cf_inode = (CFNumberRef)CFDictionaryGetValue(path_info_dict, kFSEventStreamEventExtendedFileIDKey);
        unsigned long inode;
        CFNumberGetValue(cf_inode, kCFNumberLongType, &inode);
        xu_char_t const* filepath = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);
#    else
        xu_char_t const* filepath = ((xu_char_t const**)event_paths)[i];
#    endif

        // get file path
        FSEventStreamEventFlags flags = event_flags[i];
        xu_fwatcher_event_t     event;
        if (filepath)
            xu_strlcpy(event.filepath, filepath, XU_PATH_MAXN);
        else
            event.filepath[0] = '\0';

        // we need filter file path if not recursion
        xu_bool_t matched = xu_false;
        if (watchitem->recursion)
            matched = xu_true;
        else
        {
            xu_char_t const* p = xu_strrchr(event.filepath, '/');
            if (p && p - event.filepath == xu_strlen(watchitem->watchdir)) matched = xu_true;
        }
        xu_check_continue(matched);

        // filter need events
        if (flags & kFSEventStreamEventFlagItemCreated)
            event.event = XU_FWATCHER_EVENT_CREATE;
        else if (flags & kFSEventStreamEventFlagItemRemoved)
            event.event = XU_FWATCHER_EVENT_DELETE;
        else if (flags & kFSEventStreamEventFlagItemModified)
            event.event = XU_FWATCHER_EVENT_MODIFY;
        else if (flags & kFSEventStreamEventFlagItemRenamed)
        {
            // the parent directory is changed
            event.event = XU_FWATCHER_EVENT_MODIFY;
            xu_path_directory(filepath, event.filepath, XU_PATH_MAXN);
        }

        // add event to queue
        if (event.event)
        {
            xu_spinlock_enter(&fwatcher->lock);
            if (!xu_queue_full(fwatcher->events_queue)) xu_queue_put(fwatcher->events_queue, &event);
            xu_spinlock_leave(&fwatcher->lock);
            has_events = xu_true;
        }
    }

    // notify events
    if (has_events) xu_semaphore_post(fwatcher->semaphore, 1);
}

static xu_bool_t xu_fwatcher_item_init(xu_fwatcher_t* fwatcher, xu_char_t const* watchdir,
                                       xu_fwatcher_item_t* watchitem)
{
    // check
    xu_assert_and_check_return_val(fwatcher && watchdir && watchitem, xu_false);

    // get path array
    CFStringRef pathstr    = CFStringCreateWithCString(xu_null, watchdir, kCFStringEncodingUTF8);
    CFArrayRef  path_array = CFArrayCreate(xu_null, (xu_cpointer_t*)&pathstr, 1, &kCFTypeArrayCallBacks);

    // init context
    FSEventStreamContext* context = &watchitem->context;
    context->version              = 0;
    context->info                 = watchitem;
    context->retain               = xu_null;
    context->release              = xu_null;
    context->copyDescription      = xu_null;

    // attach watchdir
    watchitem->fwatcher = (xu_fwatcher_ref_t)fwatcher;
    watchitem->watchdir = watchdir;

    // create fsevent stream
    FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer;
#    if defined(HAVE_MACOS_GE_10_13)
    flags |= kFSEventStreamCreateFlagUseExtendedData;
    flags |= kFSEventStreamCreateFlagUseCFTypes;
#    endif
    watchitem->stream = FSEventStreamCreate(xu_null, xu_fwatcher_item_callback, context, path_array,
                                            kFSEventStreamEventIdSinceNow, 0, flags);

    // creating dispatch queue
    watchitem->fsevents_queue = dispatch_queue_create("fswatch_event_queue", xu_null);
    FSEventStreamSetDispatchQueue(watchitem->stream, watchitem->fsevents_queue);

    // start stream
    FSEventStreamStart(watchitem->stream);

    // free path array
    CFRelease(pathstr);
    CFRelease(path_array);
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

        // init watch items
        fwatcher->watchitems = xu_hash_map_init(
            0, xu_element_str(xu_true), xu_element_mem(sizeof(xu_fwatcher_item_t), xu_fwatcher_item_free, xu_null));
        xu_assert_and_check_break(fwatcher->watchitems);

        // init events queue
        fwatcher->events_queue = xu_queue_init(0, xu_element_mem(sizeof(xu_fwatcher_event_t), xu_null, xu_null));
        xu_assert_and_check_break(fwatcher->events_queue);

        // init semaphore
        fwatcher->semaphore = xu_semaphore_init(0);
        xu_assert_and_check_break(fwatcher->semaphore);

        // init lock
        xu_spinlock_init(&fwatcher->lock);

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
        // exit watchitems
        if (fwatcher->watchitems) xu_hash_map_exit(fwatcher->watchitems);
        fwatcher->watchitems = xu_null;

        // exit semaphore
        if (fwatcher->semaphore) xu_semaphore_exit(fwatcher->semaphore);
        fwatcher->semaphore = xu_null;

        // exit events queue
        if (fwatcher->events_queue) xu_queue_exit(fwatcher->events_queue);
        fwatcher->events_queue = xu_null;

        // exit lock
        xu_spinlock_exit(&fwatcher->lock);

        // wait watcher
        xu_free(fwatcher);
        fwatcher = xu_null;
    }
}

xu_bool_t xu_fwatcher_add(xu_fwatcher_ref_t self, xu_char_t const* watchdir, xu_bool_t recursion)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->watchitems && watchdir, xu_false);

    // file not found
    xu_file_info_t info;
    if (!xu_file_info(watchdir, &info) || info.type != XU_FILE_TYPE_DIRECTORY) return xu_false;

    // get real path, we need match file path from event callback
    xu_char_t        data[PATH_MAX];
    xu_char_t const* watchdir_real = realpath(watchdir, data);
    if (!watchdir_real) watchdir_real = watchdir;

    // this path has been added?
    xu_size_t itor = xu_hash_map_find(fwatcher->watchitems, watchdir_real);
    if (itor != xu_iterator_tail(fwatcher->watchitems)) return xu_true;

    // save watch item
    xu_fwatcher_item_t watchitem;
    xu_memset(&watchitem, 0, sizeof(xu_fwatcher_item_t));
    watchitem.recursion = recursion;
    return xu_hash_map_insert(fwatcher->watchitems, watchdir_real, &watchitem) !=
           xu_iterator_tail(fwatcher->watchitems);
}

xu_bool_t xu_fwatcher_remove(xu_fwatcher_ref_t self, xu_char_t const* watchdir)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->watchitems && watchdir, xu_false);

    // get real path, we need match file path from event callback
    xu_char_t        data[PATH_MAX];
    xu_char_t const* watchdir_real = realpath(watchdir, data);
    if (!watchdir_real) watchdir_real = watchdir;

    // remove the watchitem
    xu_hash_map_remove(fwatcher->watchitems, watchdir_real);
    return xu_true;
}

xu_void_t xu_fwatcher_spak(xu_fwatcher_ref_t self)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return(fwatcher && fwatcher->semaphore);

    xu_semaphore_post(fwatcher->semaphore, 1);
}

xu_long_t xu_fwatcher_wait(xu_fwatcher_ref_t self, xu_fwatcher_event_t* event, xu_long_t timeout)
{
    xu_fwatcher_t* fwatcher = (xu_fwatcher_t*)self;
    xu_assert_and_check_return_val(fwatcher && fwatcher->semaphore && fwatcher->events_queue && event, -1);

#    if defined(XU_CONFIG_MODULE_HAVE_COROUTINE) && !defined(XU_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && xu_coroutine_self())
    {
        xu_poller_object_t object;
        object.type         = XU_POLLER_OBJECT_FWATCHER;
        object.ref.fwatcher = self;
        return xu_coroutine_waitfs(&object, event, timeout);
    }
#    endif

    // get it if has events
    xu_bool_t has_events = xu_false;
    xu_spinlock_enter(&fwatcher->lock);
    if (!xu_queue_null(fwatcher->events_queue))
    {
        xu_fwatcher_event_t* e = (xu_fwatcher_event_t*)xu_queue_get(fwatcher->events_queue);
        if (e)
        {
            *event = *e;
            xu_queue_pop(fwatcher->events_queue);
            has_events = xu_true;
        }
    }
    xu_spinlock_leave(&fwatcher->lock);
    xu_check_return_val(!has_events, 1);

    // init watch items
    xu_for_all(xu_hash_map_item_ref_t, item, fwatcher->watchitems)
    {
        // get watch item and path
        xu_char_t const*    watchdir  = (xu_char_t const*)item->name;
        xu_fwatcher_item_t* watchitem = (xu_fwatcher_item_t*)item->data;
        xu_assert_and_check_return_val(watchitem && watchdir, -1);

        // init watch item first
        if (!watchitem->stream && !xu_fwatcher_item_init(fwatcher, watchdir, watchitem))
        {
            xu_trace_d("watch %s failed", watchdir);
            return -1;
        }
    }

    // wait events
    xu_long_t wait = xu_semaphore_wait(fwatcher->semaphore, timeout);
    xu_assert_and_check_return_val(wait >= 0, -1);
    xu_check_return_val(wait > 0, 0);

    // get event
    xu_spinlock_enter(&fwatcher->lock);
    if (!xu_queue_null(fwatcher->events_queue))
    {
        xu_fwatcher_event_t* e = (xu_fwatcher_event_t*)xu_queue_get(fwatcher->events_queue);
        if (e)
        {
            *event = *e;
            xu_queue_pop(fwatcher->events_queue);
            has_events = xu_true;
        }
    }
    xu_spinlock_leave(&fwatcher->lock);
    return has_events ? 1 : 0;
}
#endif
