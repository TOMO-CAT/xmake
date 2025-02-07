#pragma once

/* *******************************************************
 * includes
 */
#include "xutil/algorithm/find_if.h"
#include "xutil/algorithm/for.h"
#include "xutil/algorithm/remove_first_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/container/queue.h"
#include "xutil/platform/path.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 */

// the fwatcher ref type
typedef __xu_typeref__(fwatcher);

// the fwatcher event enum
typedef enum __xu_fwatcher_event_e
{
    XU_FWATCHER_EVENT_NONE   = 0,
    XU_FWATCHER_EVENT_MODIFY = 1,
    XU_FWATCHER_EVENT_CREATE = 2,
    XU_FWATCHER_EVENT_DELETE = 4

} xu_fwatcher_event_e;

// the fwatcher event type
typedef struct __xu_fwatcher_event_t
{
    xu_size_t event;
    xu_char_t filepath[XU_PATH_MAXN];

} xu_fwatcher_event_t;

/* *******************************************************
 * interfaces
 */

/*! init the fwatcher directory watcher
 *
 * @return              the fwatcher
 */
xu_fwatcher_ref_t xu_fwatcher_init(xu_noarg_t);

/*! exit the fwatcher
 *
 * @param fwatcher      the fwatcher
 */
xu_void_t xu_fwatcher_exit(xu_fwatcher_ref_t fwatcher);

/*! add the watched directory, we can just watch single-level directory
 *
 * @param fwatcher      the fwatcher
 * @param watchdir      the watched directory
 * @param recursion     is recursion?
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_fwatcher_add(xu_fwatcher_ref_t fwatcher, xu_char_t const* watchdir, xu_bool_t recursion);

/*! remove the watched directory, we can just watch single-level directory
 *
 * @param fwatcher      the fwatcher
 * @param watchdir      the watched directory
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_fwatcher_remove(xu_fwatcher_ref_t fwatcher, xu_char_t const* watchdir);

/*! spank the fwatcher, break the xu_fwatcher_wait() and return all events
 *
 * @param fwatcher      the fwatcher
 */
xu_void_t xu_fwatcher_spak(xu_fwatcher_ref_t fwatcher);

/*! wait the fwatcher event
 *
 * @param fwatcher      the fwatcher
 * @param event         the event
 * @param timeout       the timeout, infinity: -1
 *
 * @return              > 0: has event, 0: timeout, -1: failed
 */
xu_long_t xu_fwatcher_wait(xu_fwatcher_ref_t fwatcher, xu_fwatcher_event_t* event, xu_long_t timeout);
