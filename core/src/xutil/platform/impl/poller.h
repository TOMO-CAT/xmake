#pragma once

#include "xutil/platform/impl/pollerdata.h"
#include "xutil/platform/poller.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */

// get the poller object type from the private pointer
#define xu_poller_priv_get_object_type(ptr)                                                                            \
    (((xu_size_t)(ptr) & ((xu_size_t)0x1 << (XU_CPU_BITSIZE - 1))) ? XU_POLLER_OBJECT_PIPE : XU_POLLER_OBJECT_SOCK)

// get the original private pointer
#define xu_poller_priv_get_original(ptr) ((xu_cpointer_t)((xu_size_t)(ptr) & ~((xu_size_t)0x1 << (XU_CPU_BITSIZE - 1))))

/* *******************************************************
 * types
 */

// the process poller ref type
typedef __xu_typeref__(poller_process);

// the fwatcher poller ref type
typedef __xu_typeref__(poller_fwatcher);

// the poller type
typedef struct __xu_poller_t
{
    // the user private data
    xu_cpointer_t priv;

    // the poller type
    xu_uint16_t type;

    // the supported events
    xu_uint16_t supported_events;

#ifndef XU_CONFIG_MICRO_ENABLE
    // the process poller
    xu_poller_process_ref_t process_poller;

    // the fwatcher poller
    xu_poller_fwatcher_ref_t fwatcher_poller;
#endif

    /* exit poller
     *
     * @param poller         the poller
     */
    xu_void_t (*exit)(struct __xu_poller_t* poller);

    /* kill all waited events, xu_poller_wait() will return -1
     *
     * @param poller         the poller
     */
    xu_void_t (*kill)(struct __xu_poller_t* poller);

    /* spak the poller, break the xu_poller_wait() and return all events
     *
     * @param poller         the poller
     */
    xu_void_t (*spak)(struct __xu_poller_t* poller);

    /* wait events for all objects
     *
     * @param poller         the poller
     * @param func           the events function
     * @param timeout        the timeout, infinity: -1
     *
     * @return               > 0: the events number, 0: timeout, -1: failed
     */
    xu_long_t (*wait)(struct __xu_poller_t* poller, xu_poller_event_func_t func, xu_long_t timeout);

    /* insert socket to poller
     *
     * @param poller         the poller
     * @param object         the poller object
     * @param events         the poller events
     * @param priv           the private data
     *
     * @return               xu_true or xu_false
     */
    xu_bool_t (*insert)(struct __xu_poller_t* poller, xu_poller_object_ref_t object, xu_size_t events,
                        xu_cpointer_t priv);

    /* remove socket from poller
     *
     * @param poller         the poller
     * @param object         the poller object
     *
     * @return               xu_true or xu_false
     */
    xu_bool_t (*remove)(struct __xu_poller_t* poller, xu_poller_object_ref_t object);

    /* modify events for the given socket
     *
     * @param poller         the poller
     * @param object         the poller object
     * @param events         the poller events
     * @param priv           the private data
     *
     * @return               xu_true or xu_false
     */
    xu_bool_t (*modify)(struct __xu_poller_t* poller, xu_poller_object_ref_t object, xu_size_t events,
                        xu_cpointer_t priv);

} xu_poller_t;

/* *******************************************************
 * inline implementation
 */

// set the object type to the private pointer
static __xu_inline__ xu_cpointer_t xu_poller_priv_set_object_type(xu_poller_object_ref_t object, xu_cpointer_t ptr)
{
    // must be a valid pointer address
    xu_assert(!((xu_size_t)ptr & ((xu_size_t)0x1 << (XU_CPU_BITSIZE - 1))));
    return object->type == XU_POLLER_OBJECT_PIPE
               ? (xu_cpointer_t)((xu_size_t)ptr | ((xu_size_t)0x1 << (XU_CPU_BITSIZE - 1)))
               : ptr;
}
