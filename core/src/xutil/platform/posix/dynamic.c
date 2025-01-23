#include "xutil/platform/dynamic.h"
#include "xutil/macros/assert.h"
#include <dlfcn.h>

/* *******************************************************
 *  implementation
 */
xu_dynamic_ref_t xu_dynamic_init(xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(name, xu_null);

    // clear error
    dlerror();

    // open
    xu_handle_t dynamic = dlopen(name, RTLD_LAZY);

    // error?
    if (dlerror())
    {
        if (dynamic) dlclose(dynamic);
        dynamic = xu_null;
    }

    // ok?
    return (xu_dynamic_ref_t)dynamic;
}
xu_void_t xu_dynamic_exit(xu_dynamic_ref_t dynamic)
{
    // check
    xu_assert_and_check_return(dynamic);

    // close it
    dlclose((xu_handle_t)dynamic);
    xu_assert(!dlerror());
}
xu_pointer_t xu_dynamic_func(xu_dynamic_ref_t dynamic, xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(dynamic && name, xu_null);

    // the func
    return (xu_pointer_t)dlsym((xu_handle_t)dynamic, name);
}
xu_pointer_t xu_dynamic_pvar(xu_dynamic_ref_t dynamic, xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(dynamic && name, xu_null);

    // the variable address
    return (xu_pointer_t)dlsym((xu_handle_t)dynamic, name);
}
