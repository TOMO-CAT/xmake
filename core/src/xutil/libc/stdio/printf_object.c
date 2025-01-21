/* *******************************************************
 *  trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "printf_object"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/libc/stdio/printf_object.h"

/* *******************************************************
 *  types
 * *******************************************************
 */

// the printf object entry type
typedef struct __xu_printf_object_entry_t
{
    // the format name
    xu_char_t const* name;

    // the format func
    xu_printf_object_func_t func;

} xu_printf_object_entry_t;

/* *******************************************************
 *  globals
 * *******************************************************
 */

// the entry list
static xu_printf_object_entry_t* g_list = xu_null;

// the entry size
static xu_size_t g_size = 0;

// the entry maxn
static xu_size_t g_maxn = 16;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static xu_long_t xu_printf_object_comp(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t data)
{
    // the entry
    xu_printf_object_entry_t* entry = (xu_printf_object_entry_t*)item;
    xu_assert(entry && data);

    // comp
    return xu_strcmp(entry->name, (xu_char_t const*)data);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
xu_void_t xu_printf_object_register(xu_char_t const* name, xu_printf_object_func_t func)
{
    // check
    xu_assert_and_check_return(name && g_maxn);
    xu_assert_and_check_return(xu_strlen(name) < XU_PRINTF_OBJECT_NAME_MAXN);

    // init entries
    if (!g_list) g_list = xu_nalloc_type(g_maxn, xu_printf_object_entry_t);
    xu_assert_and_check_return(g_list);

    // full? grow it
    if (g_size >= g_maxn)
    {
        // update maxn
        g_maxn = g_size + 16;

        // resize list
        g_list = (xu_printf_object_entry_t*)xu_ralloc(g_list, g_maxn * sizeof(xu_printf_object_entry_t));
        xu_assert_and_check_return(g_list);
    }

    // find it
    xu_size_t i = 0;
    xu_size_t n = g_size;
    xu_long_t r = -1;
    for (i = 0; i < n; i++)
        if ((r = xu_strcmp(name, g_list[i].name)) <= 0) break;

    // same? update it
    if (!r)
        g_list[i].func = func;
    else
    {
        // move it for inserting
        if (i < n) xu_memmov(g_list + i + 1, g_list + i, (n - i) * sizeof(xu_printf_object_entry_t));

        // register it
        g_list[i].name = name;
        g_list[i].func = func;

        // update size
        g_size++;
    }
}
xu_printf_object_func_t xu_printf_object_find(xu_char_t const* name)
{
    // check
    xu_assert_and_check_return_val(g_list && name, xu_null);

    // init the iterator
    xu_array_iterator_t array_iterator;
    xu_iterator_ref_t   iterator =
        xu_array_iterator_init_mem(&array_iterator, g_list, g_size, sizeof(xu_printf_object_entry_t));
    xu_assert_and_check_return_val(iterator, xu_null);

    // find it
    xu_size_t itor = xu_binary_find_all_if(iterator, xu_printf_object_comp, name);
    xu_check_return_val(itor != xu_iterator_tail(iterator), xu_null);

    // ok?
    return itor < g_size ? g_list[itor].func : xu_null;
}
xu_void_t xu_printf_object_exit()
{
    // exit list
    if (g_list) xu_free(g_list);
    g_list = xu_null;

    // exit size
    g_size = 0;
    g_maxn = 0;
}
#ifdef __xu_debug__
xu_void_t xu_printf_object_dump(xu_noarg_t)
{
    // check
    xu_assert_and_check_return(g_list);

    // trace
    xu_trace_i("");

    // done
    xu_size_t i = 0;
    xu_size_t n = g_size;
    for (i = 0; i < n; i++)
    {
        // trace
        xu_trace_i("format: %s", g_list[i].name);
    }
}
#endif
