#pragma once

#include "xutil/container/single_list_entry.h"
#include "xutil/prefix.h"

/* *******************************************************
 * macros
 */
// the thread local initial value
#define XU_THREAD_LOCAL_INIT                                                                                           \
    {                                                                                                                  \
        {0}, 0                                                                                                         \
    }

/* *******************************************************
 * types
 */

/*! the thread local free function type
 *
 * @param priv      the thread local private data
 */
typedef xu_void_t (*xu_thread_local_free_t)(xu_cpointer_t priv);

// the thread local type
typedef struct __xu_thread_local_t
{
    // the list entry
    xu_single_list_entry_t entry;

    // have been initialized?
    xu_bool_t inited;

    // the atomice lock of once function
    xu_atomic32_t once;

    // the free function
    xu_thread_local_free_t free;

    /* the private data space for implementation
     *
     * - sizeof(pthread_key_t) * 2 for pthread key
     */
    xu_byte_t priv[16];

} xu_thread_local_t, *xu_thread_local_ref_t;

/* *******************************************************
 * interfaces
 */

/*! init a thread local
 *
 * @note support to be called repeatedly, thread safely and only init it once
 *
 * @code

    // the thread local free function
    static xu_void_t xu_thread_local_free_func(xu_cpointer_t priv)
    {
        if (priv) xu_free(priv);
    }

    // the thread function
    static xu_cpointer_t th_thread_func(xu_cpointer_t priv)
    {
        // init the thread local, only once
        static xu_thread_local_t s_local = XU_THREAD_LOCAL_INIT;
        if (!xu_thread_local_init(&s_local, xu_thread_local_free_func)) return xu_null;

        // get the thread local data
        xu_char_t const* data = xu_null;
        if (!(data = xu_thread_local_get(&s_local)))
        {
            xu_char_t const* cstr = xu_strdup("hello");
            if (xu_thread_local_set(&s_local, cstr))
                data = cstr;
            else xu_free(cstr);
        }

        // trace
        xu_trace_i("data: %s", data);

        ...
    }

 * @endcode
 *
 * @param local         the global or static thread local (need be initialized as XU_THREAD_LOCAL_INIT)
 * @param func          the thread local free function
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_local_init(xu_thread_local_ref_t local, xu_thread_local_free_t func);

/*! exit the thread local (optional)
 *
 * @note this local will be exited automaticaly after xu_exit() was called
 *       and we call also manually call this function to exit the thread local in earlier time
 *
 * @param local         the thread local reference
 */
xu_void_t xu_thread_local_exit(xu_thread_local_ref_t local);

/*! has thead local data on the current thread?
 *
 * @param local         the thread local reference
 *
 * @return              the thread local private data
 */
xu_bool_t xu_thread_local_has(xu_thread_local_ref_t local);

/*! get thead local data from the current thread
 *
 * @param local         the thread local reference
 *
 * @return              the thread local private data
 */
xu_pointer_t xu_thread_local_get(xu_thread_local_ref_t local);

/*! set thead local data to the current thread
 *
 * @note this data will be freed automaticaly after it's thread was returned
 *
 * @param local         the thread local reference
 * @param priv          the thread local private data
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_thread_local_set(xu_thread_local_ref_t local, xu_cpointer_t priv);
