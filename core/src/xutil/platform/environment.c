#include "xutil/platform/environment.h"
#include "xutil/container/hash_set.h"
#include "xutil/platform/path.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the separator
#define TM_ENVIRONMENT_SEP ':'

/* *******************************************************
 * implementation
 * *******************************************************
 */
#include "xutil/platform/libc/environment.c"
xu_environment_ref_t xu_environment_init()
{
    // init environment
    return xu_vector_init(8, xu_element_str(xu_true));
}
xu_void_t xu_environment_exit(xu_environment_ref_t environment)
{
    // exit environment
    if (environment) xu_vector_exit(environment);
}
xu_size_t        xu_environment_size(xu_environment_ref_t environment) { return xu_vector_size(environment); }
xu_char_t const* xu_environment_at(xu_environment_ref_t environment, xu_size_t index)
{
    // check
    xu_assert_and_check_return_val(environment, xu_null);

    // get the value
    return (index < xu_vector_size(environment)) ? (xu_char_t const*)xu_iterator_item(environment, index) : xu_null;
}
xu_bool_t xu_environment_replace(xu_environment_ref_t environment, xu_char_t const* value)
{
    // check
    xu_assert_and_check_return_val(environment, xu_false);

    // clear it first
    xu_vector_clear(environment);

    // insert value
    if (value && *value != '\0') xu_vector_insert_tail(environment, value);

    // ok
    return xu_true;
}
xu_bool_t xu_environment_insert(xu_environment_ref_t environment, xu_char_t const* value, xu_bool_t to_head)
{
    // check
    xu_assert_and_check_return_val(environment && value, xu_false);
    xu_check_return_val(*value != '\0', xu_true);

    // insert value into the head
    if (to_head) xu_vector_insert_head(environment, value);
    // insert value into the tail
    else
        xu_vector_insert_tail(environment, value);

    // ok
    return xu_true;
}
xu_size_t xu_environment_get(xu_char_t const* name, xu_char_t* values, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(name && values && maxn, 0);

    // init values
    xu_string_t valuestrs;
    if (!xu_string_init(&valuestrs)) return 0;

    // init environment
    xu_environment_ref_t environment = xu_environment_init();
    if (environment)
    {
        // load variable
        if (xu_environment_load(environment, name))
        {
            // make values
            xu_bool_t is_first = xu_true;
            xu_for_all_if(xu_char_t const*, value, environment, value)
            {
                // append separator
                if (!is_first)
                    xu_string_chrcat(&valuestrs, TM_ENVIRONMENT_SEP);
                else
                    is_first = xu_false;

                // append value
                xu_string_cstrcat(&valuestrs, value);
            }
        }

        // exit environment
        xu_environment_exit(environment);
    }

    // save result
    xu_size_t        size = xu_string_size(&valuestrs);
    xu_char_t const* cstr = xu_string_cstr(&valuestrs);
    if (size && cstr)
    {
        // copy it
        size = xu_strlcpy(values, cstr, maxn);
        xu_assert(size < maxn);
    }

    // exit values
    xu_string_exit(&valuestrs);

    // ok?
    return size;
}
xu_bool_t xu_environment_set(xu_char_t const* name, xu_char_t const* values)
{
    // check
    xu_assert_and_check_return_val(name && values, xu_false);

    // find the first separator position
    xu_bool_t        ok = xu_false;
    xu_char_t const* p  = values ? xu_strchr(values, TM_ENVIRONMENT_SEP) : xu_null;
    if (p)
    {
        // init filter
        xu_hash_set_ref_t filter = xu_hash_set_init(8, xu_element_str(xu_true));

        // init environment
        xu_char_t            data[XU_PATH_MAXN];
        xu_environment_ref_t environment = xu_environment_init();
        if (environment)
        {
            // make environment
            xu_char_t const* b = values;
            xu_char_t const* e = b + xu_strlen(values);
            do
            {
                // not empty?
                if (b < p)
                {
                    // the size
                    xu_size_t size = xu_min(p - b, sizeof(data) - 1);

                    // copy it
                    xu_strncpy(data, b, size);
                    data[size] = '\0';

                    // have been not inserted?
                    if (!filter || !xu_hash_set_get(filter, data))
                    {
                        // append the environment
                        xu_environment_insert(environment, data, xu_false);

                        // save it to the filter
                        xu_hash_set_insert(filter, data);
                    }
                }

                // end?
                xu_check_break(p + 1 < e);

                // find the next separator position
                b = p + 1;
                p = xu_strchr(b, TM_ENVIRONMENT_SEP);
                if (!p) p = e;

            } while (1);

            // set environment variables
            ok = xu_environment_save(environment, name);

            // exit environment
            xu_environment_exit(environment);
        }

        // exit filter
        if (filter) xu_hash_set_exit(filter);
        filter = xu_null;
    }
    // only one?
    else
    {
        // set environment variables
        xu_environment_ref_t environment = xu_environment_init();
        if (environment)
        {
            // append the environment
            xu_environment_insert(environment, values, xu_false);

            // set environment variables
            ok = xu_environment_save(environment, name);

            // exit environment
            xu_environment_exit(environment);
        }
    }

    // ok?
    return ok;
}
xu_bool_t xu_environment_add(xu_char_t const* name, xu_char_t const* values, xu_bool_t to_head)
{
    // check
    xu_assert_and_check_return_val(name && values, xu_false);

    // find the first separator position
    xu_bool_t        ok = xu_false;
    xu_char_t const* p  = values ? xu_strchr(values, TM_ENVIRONMENT_SEP) : xu_null;
    if (p)
    {
        // init filter
        xu_hash_set_ref_t filter = xu_hash_set_init(8, xu_element_str(xu_true));

        // init environment
        xu_char_t            data[XU_PATH_MAXN];
        xu_environment_ref_t environment = xu_environment_init();
        if (environment)
        {
            // load the previous values
            xu_environment_load(environment, name);

            // make environment
            xu_char_t const* b = values;
            xu_char_t const* e = b + xu_strlen(values);
            do
            {
                // not empty?
                if (b < p)
                {
                    // the size
                    xu_size_t size = xu_min(p - b, sizeof(data) - 1);

                    // copy it
                    xu_strncpy(data, b, size);
                    data[size] = '\0';

                    // have been not inserted?
                    if (!filter || !xu_hash_set_get(filter, data))
                    {
                        // append the environment
                        xu_environment_insert(environment, data, to_head);

                        // save it to the filter
                        xu_hash_set_insert(filter, data);
                    }
                }

                // end?
                xu_check_break(p + 1 < e);

                // find the next separator position
                b = p + 1;
                p = xu_strchr(b, TM_ENVIRONMENT_SEP);
                if (!p) p = e;

            } while (1);

            // set environment variables
            ok = xu_environment_save(environment, name);

            // exit environment
            xu_environment_exit(environment);
        }

        // exit filter
        if (filter) xu_hash_set_exit(filter);
        filter = xu_null;
    }
    // only one?
    else
    {
        // set environment variables
        xu_environment_ref_t environment = xu_environment_init();
        if (environment)
        {
            // load the previous values
            xu_environment_load(environment, name);

            // append the environment
            xu_environment_insert(environment, values, to_head);

            // set environment variables
            ok = xu_environment_save(environment, name);

            // exit environment
            xu_environment_exit(environment);
        }
    }

    // ok?
    return ok;
}
#ifdef __xu_debug__
xu_void_t xu_environment_dump(xu_environment_ref_t environment, xu_char_t const* name)
{
    // trace
    xu_trace_i("%s:", name);

    // dump values
    xu_for_all_if(xu_char_t const*, value, environment, value)
    {
        // trace
        xu_trace_i("    %s", value);
    }
}
#endif
