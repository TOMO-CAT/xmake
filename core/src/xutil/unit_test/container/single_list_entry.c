#include "xutil/unit_test/ut.h"

/* *******************************************************
 * types
 */

// the demo entry type
typedef struct __xu_demo_entry_t
{
    // the list entry
    xu_single_list_entry_t entry;

    // the data
    xu_size_t data;

} xu_demo_entry_t;

/* *******************************************************
 * comparer
 */
static xu_void_t xu_demo_entry_copy(xu_pointer_t litem, xu_pointer_t ritem)
{
    // check
    xu_assert(litem && ritem);

    // copy it
    ((xu_demo_entry_t*)litem)->data = ((xu_demo_entry_t*)ritem)->data;
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_container_single_list_entry_main(xu_int_t argc, xu_char_t** argv)
{
    // init the entries
    xu_demo_entry_t entries[12] = {{{0}, 0}, {{0}, 1}, {{0}, 2}, {{0}, 3}, {{0}, 4},  {{0}, 5},
                                   {{0}, 6}, {{0}, 7}, {{0}, 8}, {{0}, 9}, {{0}, 10}, {{0}, 11}};

    // init the list
    xu_single_list_entry_head_t list;
    xu_single_list_entry_init(&list, xu_demo_entry_t, entry, xu_demo_entry_copy);

    // insert entries
    xu_single_list_entry_insert_tail(&list, &entries[5].entry);
    xu_single_list_entry_insert_tail(&list, &entries[6].entry);
    xu_single_list_entry_insert_tail(&list, &entries[7].entry);
    xu_single_list_entry_insert_tail(&list, &entries[8].entry);
    xu_single_list_entry_insert_tail(&list, &entries[9].entry);
    xu_single_list_entry_insert_head(&list, &entries[4].entry);
    xu_single_list_entry_insert_head(&list, &entries[3].entry);
    xu_single_list_entry_insert_head(&list, &entries[2].entry);
    xu_single_list_entry_insert_head(&list, &entries[1].entry);
    xu_single_list_entry_insert_head(&list, &entries[0].entry);

    // the entry
    xu_demo_entry_t* entry = (xu_demo_entry_t*)xu_single_list_entry(&list, &entries[5].entry);
    xu_trace_i("entry: %lu", entry->data);
    xu_trace_i("");

    // walk it
    xu_trace_i("insert: %lu", xu_single_list_entry_size(&list));
    xu_for_all_if(xu_demo_entry_t*, item0, xu_single_list_entry_itor(&list), item0) { xu_trace_i("%lu", item0->data); }

    // trace
    xu_trace_i("");

    // replace entries
    xu_single_list_entry_replace_head(&list, &entries[10].entry);

    // walk it
    xu_trace_i("replace: %lu", xu_single_list_entry_size(&list));
    xu_for_all_if(xu_demo_entry_t*, item1, xu_single_list_entry_itor(&list), item1) { xu_trace_i("%lu", item1->data); }

    // trace
    xu_trace_i("");

    // remove entries
    xu_single_list_entry_remove_head(&list);

    // walk it
    xu_trace_i("remove: %lu", xu_single_list_entry_size(&list));
    xu_for_all_if(xu_demo_entry_t*, item2, xu_single_list_entry_itor(&list), item2) { xu_trace_i("%lu", item2->data); }

    // trace
    xu_trace_i("");

    // clear entries
    xu_single_list_entry_clear(&list);

    // walk it
    xu_trace_i("clear: %lu", xu_single_list_entry_size(&list));
    xu_for_all_if(xu_demo_entry_t*, item5, xu_single_list_entry_itor(&list), item5) { xu_trace_i("%lu", item5->data); }

    // exit the list
    xu_single_list_entry_exit(&list);

    // trace
    xu_trace_i("");
    return 0;
}
