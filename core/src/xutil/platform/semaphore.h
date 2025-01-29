#pragma once

#include "xutil/prefix.h"

/*! init semaphore
 *
 * @param value     the initial semaphore value
 *
 * @return          the semaphore
 */
xu_semaphore_ref_t xu_semaphore_init(xu_size_t value);

/*! exit semaphore
 *
 * @param semaphore the semaphore
 */
xu_void_t xu_semaphore_exit(xu_semaphore_ref_t semaphore);

/*! post semaphore
 *
 * @param semaphore the semaphore
 * @param post      the post semaphore value
 *
 * @return          xu_true or xu_false
 */
xu_bool_t xu_semaphore_post(xu_semaphore_ref_t semaphore, xu_size_t post);

/*! the semaphore value
 *
 * @param semaphore the semaphore
 *
 * @return          >= 0: the semaphore value, -1: failed
 */
xu_long_t xu_semaphore_value(xu_semaphore_ref_t semaphore);

/*! wait semaphore
 *
 * @param semaphore the semaphore
 * @param timeout   the timeout
 *
 * @return          ok: 1, timeout or interrupted: 0, fail: -1
 */
xu_long_t xu_semaphore_wait(xu_semaphore_ref_t semaphore, xu_long_t timeout);
