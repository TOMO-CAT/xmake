#pragma once

#include "xutil/container/iterator.h"
#include "xutil/prefix.h"

/* *******************************************************
 * types
 * *******************************************************
 */

// the environment variable ref type
typedef xu_iterator_ref_t xu_environment_ref_t;

/* *******************************************************
 * interfaces
 * *******************************************************
 */

/*! init the environment variable
 *
 * @return              the environment variable
 */
xu_environment_ref_t xu_environment_init(xu_noarg_t);

/*! exit the environment variable
 *
 * @param environment   the environment variable
 */
xu_void_t xu_environment_exit(xu_environment_ref_t environment);

/*! the environment variable count
 *
 * @param environment   the environment variable
 *
 * @return              the environment variable count
 */
xu_size_t xu_environment_size(xu_environment_ref_t environment);

/*! load the environment variable from the given name
 *
 * @code
 *
    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // load variable
        if (xu_environment_load(environment, "PATH"))
        {
            // trace
            xu_trace_i("%s:", name);

            // dump values
            xu_for_all_if (xu_char_t const*, value, environment, value)
            {
                // trace
                xu_trace_i("    %s", value);
            }
        }

        // exit environment
        xu_environment_exit(environment);
    }
 * @endcode
 *
 * @param environment   the environment variable
 * @param name          the variable name
 *
 * @return              the count of the variable value
 */
xu_size_t xu_environment_load(xu_environment_ref_t environment, xu_char_t const* name);

/*! save the environment variable and will overwrite it
 *
 * we will remove this environment variable if environment is null or empty
 *
 * @code
 *
    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // insert values
        xu_environment_insert(environment, "/xxx/0", xu_false);
        xu_environment_insert(environment, "/xxx/1", xu_false);
        xu_environment_insert(environment, "/xxx/2", xu_false);
        xu_environment_insert(environment, "/xxx/3", xu_false);

        // save variable
        xu_environment_save(environment, "PATH");

        // exit environment
        xu_environment_exit(environment);
    }

 * @endcode
 *
 * @param environment   the environment variable
 * @param name          the variable name
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_save(xu_environment_ref_t environment, xu_char_t const* name);

/*! get the environment variable from the given index
 *
 * @code
 *
    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // load variable
        if (xu_environment_load(environment, "PATH"))
        {
            xu_char_t const* value = xu_environment_at(environment, 0);
            if (value)
            {
                // ...
            }
        }

        // exit environment
        xu_environment_exit(environment);
    }
 * @endcode
 *
 *
 * @param environment   the environment variable
 * @param index         the variable index
 *
 * @return              the variable value
 */
xu_char_t const* xu_environment_at(xu_environment_ref_t environment, xu_size_t index);

/*! replace the environment variable and will overwrite it
 *
 * we will clear environment and overwrite it
 *
 * @param environment   the environment variable
 * @param value         the variable value, will clear it if be null
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_replace(xu_environment_ref_t environment, xu_char_t const* value);

/*! set the environment variable
 *
 * @param environment   the environment variable
 * @param value         the variable value
 * @param to_head       insert value into the head?
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_insert(xu_environment_ref_t environment, xu_char_t const* value, xu_bool_t to_head);

#ifdef __xu_debug__
/*! dump the environment variable
 *
 * @param environment   the environment variable
 * @param name          the variable name
 */
xu_void_t xu_environment_dump(xu_environment_ref_t environment, xu_char_t const* name);
#endif

/*! get the first environment variable value
 *
 * @code

    xu_char_t value[XU_PATH_MAXN];
    if (xu_environment_first("HOME", value, sizeof(value)))
    {
        // ...
    }

 * @endcode
 *
 * @param name          the variable name
 * @param value         the variable value
 * @param maxn          the variable value maxn
 *
 * @return              the variable value size
 */
xu_size_t xu_environment_first(xu_char_t const* name, xu_char_t* value, xu_size_t maxn);

/*! get the environment variable values
 *
 * @code

    xu_char_t value[XU_PATH_MAXN];
    if (xu_environment_get("HOME", value, sizeof(value)))
    {
        // ...
    }

 * @endcode
 *
 * @param name          the variable name
 * @param values        the variable values, separator: (';')
 * @param maxn          the variable values maxn
 *
 * @return              the variable values size
 */
xu_size_t xu_environment_get(xu_char_t const* name, xu_char_t* values, xu_size_t maxn);

/*! set the environment variable values
 *
 * we will set all values and overwrite it
 *
 * @param name          the variable name
 * @param values        the variable values, separator: (';')
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_set(xu_char_t const* name, xu_char_t const* values);

/*! add the environment variable values and not overwrite it
 *
 * @param name          the variable name
 * @param values        the variable values, separator: (';')
 * @param to_head       add value into the head?
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_add(xu_char_t const* name, xu_char_t const* values, xu_bool_t to_head);

/*! remove the given environment variable
 *
 * @param name          the variable name
 *
 * @return              xu_true or xu_false
 */
xu_bool_t xu_environment_remove(xu_char_t const* name);
