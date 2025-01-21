#include "xutil/container/hash_map.h"
#include "xutil/libc/string/string.h"
#include "xutil/libm/math.h"
#include "xutil/macros/malloc.h"
#include "xutil/utils/utils.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// index
#if XU_CPU_BIT64
#    define xu_hash_map_index_make(buck, item) (((xu_size_t)((item)&0xffffffff) << 32) | ((buck)&0xffffffff))
#    define xu_hash_map_index_buck(index) ((index)&0xffffffff)
#    define xu_hash_map_index_item(index) (((index) >> 32) & 0xffffffff)
#else
#    define xu_hash_map_index_make(buck, item) (((xu_size_t)((item)&0xffff) << 16) | ((buck)&0xffff))
#    define xu_hash_map_index_buck(index) ((index)&0xffff)
#    define xu_hash_map_index_item(index) (((index) >> 16) & 0xffff)
#endif

// the self bucket default size
#ifdef __xu_small__
#    define XU_HASH_MAP_BUCKET_SIZE_DEFAULT XU_HASH_MAP_BUCKET_SIZE_MICRO
#else
#    define XU_HASH_MAP_BUCKET_SIZE_DEFAULT XU_HASH_MAP_BUCKET_SIZE_SMALL
#endif

// the self bucket maximum size
#define XU_HASH_MAP_BUCKET_MAXN (1 << 16)

// the self bucket item maximum size
#define XU_HASH_MAP_BUCKET_ITEM_MAXN (1 << 16)

/* *******************************************************
 * types
 * *******************************************************
 */

// the hash map item list type
typedef struct __xu_hash_map_item_list_t
{
    // the list size
    xu_size_t size;

    // the list maxn
    xu_size_t maxn;

} xu_hash_map_item_list_t;

// the hash map type
typedef struct __xu_hash_map_t
{
    // the item itor
    xu_iterator_t itor;

    // the hash list
    xu_hash_map_item_list_t** hash_list;

    // the hash list size
    xu_size_t hash_size;

    // the current item for iterator
    xu_hash_map_item_t item;

    // the item size
    xu_size_t item_size;

    // the item maxn
    xu_size_t item_maxn;

    // the item grow
    xu_size_t item_grow;

    // the element for name
    xu_element_t element_name;

    // the element for data
    xu_element_t element_data;

} xu_hash_map_t;

/* *******************************************************
 * private implementation
 * *******************************************************
 */
#if 0
// linear finder
static xu_bool_t xu_hash_map_item_find(xu_hash_map_t* hash_map, xu_cpointer_t name, xu_size_t* pbuck, xu_size_t* pitem)
{
    xu_assert_and_check_return_val(hash_map && hash_map->hash_list && hash_map->hash_size, xu_false);

    // get step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return_val(step, xu_false);

    // compute hash_map from name
    xu_size_t buck = hash_map->element_name.hash(&hash_map->element_name, name, hash_map->hash_size - 1, 0);
    xu_assert_and_check_return_val(buck < hash_map->hash_size, xu_false);

    // update buck
    if (pbuck) *pbuck = buck;

    // get list
    xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
    xu_check_return_val(list && list->size, xu_false);

    // find item
    xu_long_t   r = 1;
    xu_size_t   i = 0;
    xu_size_t   n = list->size;
    for (i = 0; i < n; i++)
    {
        // get item
        xu_byte_t const* item = ((xu_byte_t*)&list[1]) + i * step;

        // compare it
        r = hash_map->element_name.comp(&hash_map->element_name, name, hash_map->element_name.data(&hash_map->element_name, item));
        if (r <= 0) break;
    }

    // update item
    if (pitem) *pitem = i;

    // ok?
    return !r? xu_true : xu_false;
}
#else
// binary finder
static xu_bool_t xu_hash_map_item_find(xu_hash_map_t* hash_map, xu_cpointer_t name, xu_size_t* pbuck, xu_size_t* pitem)
{
    // check
    xu_assert_and_check_return_val(hash_map && hash_map->hash_list && hash_map->hash_size, xu_false);

    // get step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return_val(step, xu_false);

    // compute hash_map from name
    xu_size_t buck = hash_map->element_name.hash(&hash_map->element_name, name, hash_map->hash_size - 1, 0);
    xu_assert_and_check_return_val(buck < hash_map->hash_size, xu_false);

    // update buck
    if (pbuck) *pbuck = buck;

    // get list
    xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
    xu_check_return_val(list && list->size, xu_false);

    // find item
    xu_long_t t = 1;
    xu_size_t l = 0;
    xu_size_t r = list->size;
    xu_size_t m = (l + r) >> 1;
    while (l < r)
    {
        // get item
        xu_byte_t const* item = ((xu_byte_t*)&list[1]) + m * step;

        // compare it
        t = hash_map->element_name.comp(&hash_map->element_name, name,
                                        hash_map->element_name.data(&hash_map->element_name, item));
        if (t < 0)
            r = m;
        else if (t > 0)
            l = m + 1;
        else
            break;

        // next
        m = (l + r) >> 1;
    }

    /* update item
     *
     * @note: m is not the prev not same item if not found and list has repeat items
     * but this hash_map not exists repeat
     *
     * @see xu_binary_pfind()
     */
    if (pitem) *pitem = m;

    // ok?
    return !t ? xu_true : xu_false;
}
#endif
static xu_bool_t xu_hash_map_item_at(xu_hash_map_t* hash_map, xu_size_t buck, xu_size_t item, xu_pointer_t* pname,
                                     xu_pointer_t* pdata)
{
    // check
    xu_assert_and_check_return_val(hash_map && hash_map->hash_list && hash_map->hash_size && buck < hash_map->hash_size,
                                   xu_false);

    // get step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return_val(step, xu_false);

    // get list
    xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
    xu_check_return_val(list && list->size && item < list->size, xu_false);

    // get name
    if (pname) *pname = hash_map->element_name.data(&hash_map->element_name, ((xu_byte_t*)&list[1]) + item * step);

    // get data
    if (pdata)
        *pdata = hash_map->element_data.data(&hash_map->element_data,
                                             ((xu_byte_t*)&list[1]) + item * step + hash_map->element_name.size);

    // ok
    return xu_true;
}
static xu_size_t xu_hash_map_itor_size(xu_iterator_ref_t iterator)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map);

    // the size
    return hash_map->item_size;
}
static xu_size_t xu_hash_map_itor_head(xu_iterator_ref_t iterator)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map);

    // find the head
    xu_size_t i = 0;
    xu_size_t n = hash_map->hash_size;
    for (i = 0; i < n; i++)
    {
        xu_hash_map_item_list_t* list = hash_map->hash_list[i];
        if (list && list->size) return xu_hash_map_index_make(i + 1, 1);
    }
    return 0;
}
static xu_size_t xu_hash_map_itor_tail(xu_iterator_ref_t iterator) { return 0; }
static xu_size_t xu_hash_map_itor_next(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && hash_map->hash_list && hash_map->hash_size);

    // the current buck and item
    xu_size_t buck = xu_hash_map_index_buck(itor);
    xu_size_t item = xu_hash_map_index_item(itor);
    xu_assert(buck && item);

    // compute index
    buck--;
    item--;
    xu_assert(buck < hash_map->hash_size && (item + 1) < XU_HASH_MAP_BUCKET_ITEM_MAXN);

    // find the next from the current buck first
    if (hash_map->hash_list[buck] && item + 1 < hash_map->hash_list[buck]->size)
        return xu_hash_map_index_make(buck + 1, item + 2);

    // find the next from the next buckets
    xu_size_t i;
    xu_size_t n = hash_map->hash_size;
    for (i = buck + 1; i < n; i++)
    {
        xu_hash_map_item_list_t* list = hash_map->hash_list[i];
        if (list && list->size) return xu_hash_map_index_make(i + 1, 1);
    }

    // tail
    return 0;
}
static xu_pointer_t xu_hash_map_itor_item(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && itor);

    // get the buck and item
    xu_size_t buck = xu_hash_map_index_buck(itor);
    xu_size_t item = xu_hash_map_index_item(itor);
    xu_assert_and_check_return_val(buck && item, xu_null);

    // get item
    if (xu_hash_map_item_at(hash_map, buck - 1, item - 1, &((xu_hash_map_t*)hash_map)->item.name,
                            &((xu_hash_map_t*)hash_map)->item.data))
        return &(hash_map->item);
    return xu_null;
}
static xu_void_t xu_hash_map_itor_copy(xu_iterator_ref_t iterator, xu_size_t itor, xu_cpointer_t item)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && hash_map->hash_list && hash_map->hash_size);

    // the buck and item
    xu_size_t b = xu_hash_map_index_buck(itor);
    xu_size_t i = xu_hash_map_index_item(itor);
    xu_assert(b && i);
    b--;
    i--;
    xu_assert(b < hash_map->hash_size);

    // step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert(step);

    // list
    xu_hash_map_item_list_t* list = hash_map->hash_list[b];
    xu_check_return(list && list->size && i < list->size);

    // note: copy data only, will destroy hash_map index if copy name
    hash_map->element_data.copy(&hash_map->element_data,
                                ((xu_byte_t*)&list[1]) + i * step + hash_map->element_name.size, item);
}
static xu_long_t xu_hash_map_itor_comp(xu_iterator_ref_t iterator, xu_cpointer_t lelement, xu_cpointer_t relement)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && hash_map->element_name.comp && lelement && relement);

    // done
    return hash_map->element_name.comp(&hash_map->element_name, ((xu_hash_map_item_ref_t)lelement)->name,
                                       ((xu_hash_map_item_ref_t)relement)->name);
}
static xu_void_t xu_hash_map_itor_remove(xu_iterator_ref_t iterator, xu_size_t itor)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && hash_map->hash_list && hash_map->hash_size);

    // buck & item
    xu_size_t buck = xu_hash_map_index_buck(itor);
    xu_size_t item = xu_hash_map_index_item(itor);
    xu_assert(buck && item);
    buck--;
    item--;
    xu_assert(buck < hash_map->hash_size);

    // the step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert(step);

    // get list
    xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
    xu_assert(list && list->size && item < list->size);

    // free item
    if (hash_map->element_name.free)
        hash_map->element_name.free(&hash_map->element_name, ((xu_byte_t*)&list[1]) + item * step);
    if (hash_map->element_data.free)
        hash_map->element_data.free(&hash_map->element_data,
                                    ((xu_byte_t*)&list[1]) + item * step + hash_map->element_name.size);

    // remove item from the list
    if (list->size > 1)
    {
        // move items
        if (item < list->size - 1)
            xu_memmov(((xu_byte_t*)&list[1]) + item * step, ((xu_byte_t*)&list[1]) + (item + 1) * step,
                      (list->size - item - 1) * step);

        // update size
        list->size--;
    }
    // remove list
    else
    {
        // free it
        xu_free(list);

        // reset
        hash_map->hash_list[buck] = xu_null;
    }

    // update the hash_map item size
    hash_map->item_size--;
}
static xu_void_t xu_hash_map_itor_nremove(xu_iterator_ref_t iterator, xu_size_t prev, xu_size_t next, xu_size_t size)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)iterator;
    xu_assert(hash_map && hash_map->hash_list && hash_map->hash_size);

    // no size
    xu_check_return(size);

    // the step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert(step);

    // the first itor
    xu_size_t itor = prev ? xu_hash_map_itor_next(iterator, prev) : xu_hash_map_itor_head(iterator);

    // the head buck and item
    xu_size_t buck_head = xu_hash_map_index_buck(itor);
    xu_size_t item_head = xu_hash_map_index_item(itor);
    xu_assert(buck_head && item_head);

    // compute index
    buck_head--;
    item_head--;
    xu_assert(buck_head < hash_map->hash_size && item_head < XU_HASH_MAP_BUCKET_ITEM_MAXN);

    // the last buck and the tail item
    xu_size_t buck_last;
    xu_size_t item_tail;
    if (next)
    {
        // next => buck and item
        buck_last = xu_hash_map_index_buck(next);
        item_tail = xu_hash_map_index_item(next);
        xu_assert(buck_last && item_tail);

        // compute index
        buck_last--;
        item_tail--;
        xu_assert(buck_last < hash_map->hash_size && item_tail < XU_HASH_MAP_BUCKET_ITEM_MAXN);
    }
    else
    {
        buck_last = hash_map->hash_size - 1;
        item_tail = -1;
    }

    // remove items: [itor, next)
    xu_size_t              buck;
    xu_size_t              item;
    xu_element_free_func_t name_free = hash_map->element_name.free;
    xu_element_free_func_t data_free = hash_map->element_data.free;
    for (buck = buck_head, item = item_head; buck <= buck_last; buck++, item = 0)
    {
        // the list
        xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
        xu_check_continue(list && list->size);

        // the tail
        xu_size_t tail = (buck == buck_last && next) ? item_tail : list->size;
        xu_assert(tail != -1);
        xu_check_continue(item < tail);

        // the data
        xu_byte_t* data = (xu_byte_t*)&list[1];

        // free items
        xu_size_t i = 0;
        for (i = item; i < tail; i++)
        {
            if (name_free) name_free(&hash_map->element_name, data + i * step);
            if (data_free) data_free(&hash_map->element_data, data + i * step + hash_map->element_name.size);
        }

        // move items
        if (buck == buck_last && tail < list->size)
            xu_memmov(data + item * step, data + tail * step, (list->size - tail) * step);

        // update the list size
        list->size -= tail - item;

        // update the item size
        hash_map->item_size -= tail - item;
    }
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_hash_map_ref_t xu_hash_map_init(xu_size_t bucket_size, xu_element_t element_name, xu_element_t element_data)
{
    // check
    xu_assert_and_check_return_val(
        element_name.size && element_name.hash && element_name.comp && element_name.data && element_name.dupl, xu_null);
    xu_assert_and_check_return_val(element_data.data && element_data.dupl && element_data.repl, xu_null);

    // check bucket size
    if (!bucket_size) bucket_size = XU_HASH_MAP_BUCKET_SIZE_DEFAULT;
    xu_assert_and_check_return_val(bucket_size <= XU_HASH_MAP_BUCKET_SIZE_LARGE, xu_null);

    // done
    xu_bool_t      ok       = xu_false;
    xu_hash_map_t* hash_map = xu_null;
    do
    {
        // make self
        hash_map = xu_malloc0_type(xu_hash_map_t);
        xu_assert_and_check_break(hash_map);

        // init self func
        hash_map->element_name = element_name;
        hash_map->element_data = element_data;

        // init operation
        static xu_iterator_op_t op = {xu_hash_map_itor_size,
                                      xu_hash_map_itor_head,
                                      xu_null,
                                      xu_hash_map_itor_tail,
                                      xu_null,
                                      xu_hash_map_itor_next,
                                      xu_hash_map_itor_item,
                                      xu_hash_map_itor_comp,
                                      xu_hash_map_itor_copy,
                                      xu_hash_map_itor_remove,
                                      xu_hash_map_itor_nremove};

        // init iterator
        hash_map->itor.priv = xu_null;
        hash_map->itor.step = sizeof(xu_hash_map_item_t);
        hash_map->itor.mode = XU_ITERATOR_MODE_FORWARD | XU_ITERATOR_MODE_MUTABLE;
        hash_map->itor.op   = &op;

        // init self size
        hash_map->hash_size = xu_align_pow2(bucket_size);
        xu_assert_and_check_break(hash_map->hash_size <= XU_HASH_MAP_BUCKET_MAXN);

        // init self list
        hash_map->hash_list = (xu_hash_map_item_list_t**)xu_nalloc0(hash_map->hash_size, sizeof(xu_size_t));
        xu_assert_and_check_break(hash_map->hash_list);

        // init item grow
        hash_map->item_grow = xu_isqrti((xu_uint32_t)bucket_size);
        if (hash_map->item_grow < 8) hash_map->item_grow = 8;
        hash_map->item_grow = xu_align_pow2(hash_map->item_grow);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (hash_map) xu_hash_map_exit((xu_hash_map_ref_t)hash_map);
        hash_map = xu_null;
    }

    // ok?
    return (xu_hash_map_ref_t)hash_map;
}
xu_void_t xu_hash_map_exit(xu_hash_map_ref_t self)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return(hash_map);

    // clear it
    xu_hash_map_clear(self);

    // free hash_map list
    if (hash_map->hash_list) xu_free(hash_map->hash_list);

    // free it
    xu_free(hash_map);
}
xu_void_t xu_hash_map_clear(xu_hash_map_ref_t self)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return(hash_map && hash_map->hash_list);

    // step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return(step);

    // clear hash_map
    xu_size_t i = 0;
    xu_size_t n = hash_map->hash_size;
    for (i = 0; i < n; i++)
    {
        xu_hash_map_item_list_t* list = hash_map->hash_list[i];
        if (list)
        {
            // free items
            if (hash_map->element_name.free || hash_map->element_data.free)
            {
                xu_size_t j = 0;
                xu_size_t m = list->size;
                for (j = 0; j < m; j++)
                {
                    xu_byte_t* item = ((xu_byte_t*)&list[1]) + j * step;
                    if (hash_map->element_name.free) hash_map->element_name.free(&hash_map->element_name, item);
                    if (hash_map->element_data.free)
                        hash_map->element_data.free(&hash_map->element_data, item + hash_map->element_name.size);
                }
            }

            // free list
            xu_free(list);
        }
        hash_map->hash_list[i] = xu_null;
    }

    // reset info
    hash_map->item_size = 0;
    hash_map->item_maxn = 0;
    xu_memset(&hash_map->item, 0, sizeof(xu_hash_map_item_t));
}
xu_pointer_t xu_hash_map_get(xu_hash_map_ref_t self, xu_cpointer_t name)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return_val(hash_map, xu_null);

    // find it
    xu_size_t buck = 0;
    xu_size_t item = 0;
    if (!xu_hash_map_item_find(hash_map, name, &buck, &item)) return xu_null;

    // get data
    xu_pointer_t data = xu_null;
    return xu_hash_map_item_at(hash_map, buck, item, xu_null, &data) ? data : xu_null;
}
xu_size_t xu_hash_map_find(xu_hash_map_ref_t self, xu_cpointer_t name)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return_val(hash_map, 0);

    // find
    xu_size_t buck = 0;
    xu_size_t item = 0;
    return xu_hash_map_item_find(hash_map, name, &buck, &item) ? xu_hash_map_index_make(buck + 1, item + 1) : 0;
}
xu_size_t xu_hash_map_insert(xu_hash_map_ref_t self, xu_cpointer_t name, xu_cpointer_t data)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return_val(hash_map, 0);

    // the step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return_val(step, 0);

    // find it
    xu_size_t buck = 0;
    xu_size_t item = 0;
    if (xu_hash_map_item_find(hash_map, name, &buck, &item))
    {
        // check
        xu_assert_and_check_return_val(buck < hash_map->hash_size, 0);

        // get list
        xu_hash_map_item_list_t* list = hash_map->hash_list[buck];
        xu_assert_and_check_return_val(list && list->size && item < list->size, 0);

        // replace data
        hash_map->element_data.repl(&hash_map->element_data,
                                    ((xu_byte_t*)&list[1]) + item * step + hash_map->element_name.size, data);
    }
    else
    {
        // check
        xu_assert_and_check_return_val(buck < hash_map->hash_size, 0);

        // get list
        xu_hash_map_item_list_t* list = hash_map->hash_list[buck];

        // insert item
        if (list)
        {
            // grow?
            if (list->size >= list->maxn)
            {
                // check
                xu_assert_and_check_return_val(hash_map->item_grow, 0);

                // resize maxn
                xu_size_t maxn = xu_align_pow2(list->maxn + hash_map->item_grow);
                xu_assert_and_check_return_val(maxn > list->maxn, 0);

                // realloc it
                list = (xu_hash_map_item_list_t*)xu_ralloc(list, sizeof(xu_hash_map_item_list_t) + maxn * step);
                xu_assert_and_check_return_val(list, 0);

                // update the hash_map item maxn
                hash_map->item_maxn += maxn - list->maxn;

                // update maxn
                list->maxn = maxn;

                // reattach list
                hash_map->hash_list[buck] = list;
            }
            xu_assert_and_check_return_val(item <= list->size && list->size < list->maxn, 0);

            // move items
            if (item != list->size)
                xu_memmov(((xu_byte_t*)&list[1]) + (item + 1) * step, ((xu_byte_t*)&list[1]) + item * step,
                          (list->size - item) * step);

            // dupl item
            list->size++;
            hash_map->element_name.dupl(&hash_map->element_name, ((xu_byte_t*)&list[1]) + item * step, name);
            hash_map->element_data.dupl(&hash_map->element_data,
                                        ((xu_byte_t*)&list[1]) + item * step + hash_map->element_name.size, data);
        }
        // create list for adding item
        else
        {
            // check
            xu_assert_and_check_return_val(hash_map->item_grow, 0);

            // make list
            list = (xu_hash_map_item_list_t*)xu_malloc0(sizeof(xu_hash_map_item_list_t) + hash_map->item_grow * step);
            xu_assert_and_check_return_val(list, 0);

            // init list
            list->size = 1;
            list->maxn = hash_map->item_grow;
            hash_map->element_name.dupl(&hash_map->element_name, ((xu_byte_t*)&list[1]), name);
            hash_map->element_data.dupl(&hash_map->element_data, ((xu_byte_t*)&list[1]) + hash_map->element_name.size,
                                        data);

            // attach list
            hash_map->hash_list[buck] = list;

            // update the hash_map item maxn
            hash_map->item_maxn += list->maxn;
        }

        // update the hash_map item size
        hash_map->item_size++;
    }

    // ok?
    return xu_hash_map_index_make(buck + 1, item + 1);
}
xu_void_t xu_hash_map_remove(xu_hash_map_ref_t self, xu_cpointer_t name)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return(hash_map);

    // find it
    xu_size_t buck = 0;
    xu_size_t item = 0;
    if (xu_hash_map_item_find(hash_map, name, &buck, &item))
        xu_hash_map_itor_remove((xu_iterator_ref_t)hash_map, xu_hash_map_index_make(buck + 1, item + 1));
}
xu_size_t xu_hash_map_size(xu_hash_map_ref_t self)
{
    // check
    xu_hash_map_t const* hash_map = (xu_hash_map_t const*)self;
    xu_assert_and_check_return_val(hash_map, 0);

    // the size
    return hash_map->item_size;
}
xu_size_t xu_hash_map_maxn(xu_hash_map_ref_t self)
{
    // check
    xu_hash_map_t const* hash_map = (xu_hash_map_t const*)self;
    xu_assert_and_check_return_val(hash_map, 0);

    // the maxn
    return hash_map->item_maxn;
}
#ifdef __xu_debug__
xu_void_t xu_hash_map_dump(xu_hash_map_ref_t self)
{
    // check
    xu_hash_map_t* hash_map = (xu_hash_map_t*)self;
    xu_assert_and_check_return(hash_map && hash_map->hash_list);

    // the step
    xu_size_t step = hash_map->element_name.size + hash_map->element_data.size;
    xu_assert_and_check_return(step);

    // trace
    xu_trace_i("");
    xu_trace_i("self: size: %lu", xu_hash_map_size(self));

    // done
    xu_size_t i = 0;
    xu_char_t name[4096];
    xu_char_t data[4096];
    for (i = 0; i < hash_map->hash_size; i++)
    {
        // the list
        xu_hash_map_item_list_t* list = hash_map->hash_list[i];
        if (list)
        {
            // trace
            xu_trace_i("buck[%u]: size: %u, maxn: %u", i, list->size, list->maxn);

            // done
            xu_size_t j = 0;
            for (j = 0; j < list->size; j++)
            {
                // the item
                xu_byte_t const* item = ((xu_byte_t*)&list[1]) + j * step;

                // the item name
                xu_pointer_t element_name = hash_map->element_name.data(&hash_map->element_name, item);

                // the item data
                xu_pointer_t element_data =
                    hash_map->element_data.data(&hash_map->element_data, item + hash_map->element_name.size);

                // trace
                if (hash_map->element_name.cstr && hash_map->element_data.cstr)
                {
                    xu_trace_i("    %s => %s",
                               hash_map->element_name.cstr(&hash_map->element_name, element_name, name, sizeof(name)),
                               hash_map->element_data.cstr(&hash_map->element_data, element_data, data, sizeof(data)));
                }
                else if (hash_map->element_name.cstr)
                {
                    xu_trace_i("    %s => %p",
                               hash_map->element_name.cstr(&hash_map->element_name, element_name, name, sizeof(name)),
                               element_data);
                }
                else if (hash_map->element_data.cstr)
                {
                    xu_trace_i("    %x => %p", element_name,
                               hash_map->element_data.cstr(&hash_map->element_data, element_data, data, sizeof(data)));
                }
                else
                {
                    xu_trace_i("    %p => %p", element_name, element_data);
                }
            }
        }
    }
}
#endif
