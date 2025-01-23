#include "xutil/platform/environment.h"
#include "xutil/algorithm/for_if.h"
#include "xutil/container/vector.h"
#include "xutil/string/string.h"

#include <stdlib.h>

/* *******************************************************
 * implementation
 */
xu_size_t xu_environment_load(xu_environment_ref_t environment, xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(environment && name, 0);

    // clear environment first
    xu_vector_clear(environment);

    // get values
    xu_char_t const* values = getenv(name);
    xu_check_return_val(values, 0);

    // init value string
    xu_string_t value;
    if (!xu_string_init(&value)) return 0;

    // done
    xu_char_t const* p = values;
    xu_char_t        c = '\0';
    while (1)
    {
        // the character
        c = *p++;

        // make value
        if (c != ':' && c)
            xu_string_chrcat(&value, c);
        else
        {
            // save value to environment
            if (xu_string_size(&value)) xu_vector_insert_tail(environment, xu_string_cstr(&value));

            // clear value
            xu_string_clear(&value);

            // end?
            xu_check_break(c);
        }
    }

    // exit value string
    xu_string_exit(&value);

    // ok?
    return xu_vector_size(environment);
}
xu_bool_t xu_environment_save(xu_environment_ref_t environment, xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(environment && name, xu_false);

    // empty? remove this environment variable
    if (!xu_vector_size(environment)) return !unsetenv(name);

    // init values string
    xu_string_t values;
    if (!xu_string_init(&values)) return xu_false;

    // make values string
    xu_for_all_if(xu_char_t const*, value, environment, value)
    {
        // the single value cannot exist ':'
        xu_assertf(!xu_strchr(value, ':'), "invalid value: %s", value);

        // append value
        xu_string_cstrcat(&values, value);
        xu_string_chrcat(&values, ':');
    }

    // strip the last ':'
    xu_string_strip(&values, xu_string_size(&values) - 1);

    // save variable
    xu_bool_t        ok;
    xu_char_t const* value_cstr = xu_string_cstr(&values);
    if (value_cstr)
        ok = !setenv(name, value_cstr, 1);
    else
        ok = !unsetenv(name);

    // exit values string
    xu_string_exit(&values);

    // ok?
    return ok;
}
xu_size_t xu_environment_first(xu_char_t const* name, xu_char_t* value, xu_size_t maxn)
{
    // check
    xu_assert_and_check_return_val(name && value && maxn, 0);

    // get it
    xu_char_t const* data = getenv(name);
    xu_check_return_val(data, 0);

    // the value size
    xu_size_t size = xu_strlen(data);
    xu_check_return_val(size, 0);

    // the space is not enough
    xu_assert_and_check_return_val(size < maxn, 0);

    // copy it
    xu_strlcpy(value, data, maxn);
    value[size] = '\0';

    // only get the first one if exists multiple values
    xu_char_t* p = xu_strchr(value, ':');
    if (p)
    {
        // strip it
        *p = '\0';

        // update size
        size = p - value;
    }

    // ok
    return size;
}
xu_bool_t xu_environment_remove(xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(name, xu_false);

    // remove it
    return !unsetenv(name);
}
