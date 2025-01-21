#pragma once

#include "xutil/macros/type.h"
#include "xutil/prefix.h"

/*! init mutex
 *
 * @return          the mutex
 */
xu_mutex_ref_t xu_mutex_init(xu_noarg_t);

/* exit mutex
 *
 * @param mutex     the mutex
 */
xu_void_t xu_mutex_exit(xu_mutex_ref_t mutex);

/* enter mutex
 *
 * @param mutex     the mutex
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_mutex_enter(xu_mutex_ref_t mutex);

/* try to enter mutex
 *
 * @param mutex     the mutex
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_mutex_enter_try(xu_mutex_ref_t mutex);

/* leave mutex
 *
 * @param mutex     the mutex
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_mutex_leave(xu_mutex_ref_t mutex);
