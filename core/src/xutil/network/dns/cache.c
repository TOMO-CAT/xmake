#include "xutil/network/dns/cache.h"
#include "xutil/algorithm/remove_if.h"
#include "xutil/container/hash_map.h"
#include "xutil/libc/string/string.h"
#include "xutil/libm/math.h"
#include "xutil/platform/cache_time.h"
#include "xutil/platform/spinlock.h"

/* *******************************************************
 * macros
 * *******************************************************
 */

// the cache maxn
#ifdef __xu_small__
#    define XU_DNS_CACHE_MAXN (64)
#else
#    define XU_DNS_CACHE_MAXN (256)
#endif

/* *******************************************************
 * types
 * *******************************************************
 */

// the dns cache type
typedef struct __xu_dns_cache_t
{
    // the hash
    xu_hash_map_ref_t hash;

    // the times
    xu_hize_t times;

    // the expired
    xu_size_t expired;

} xu_dns_cache_t;

// the dns cache addr type
typedef struct __xu_dns_cache_addr_t
{
    // the addr
    xu_ipaddr_t addr;

    // the time
    xu_size_t time;

} xu_dns_cache_addr_t;

/* *******************************************************
 * globals
 * *******************************************************
 */

// the lock
static xu_spinlock_t g_lock = XU_SPINLOCK_INIT;

// the cache
static xu_dns_cache_t g_cache = {0};

/* *******************************************************
 * helper
 * *******************************************************
 */

static __xu_inline__ xu_size_t xu_dns_cache_now() { return (xu_size_t)(xu_cache_time_spak() / 1000); }
static xu_bool_t               xu_dns_cache_clear(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_assert(item);

    // the dns cache address
    xu_dns_cache_addr_t const* caddr = (xu_dns_cache_addr_t const*)((xu_hash_map_item_ref_t)item)->data;
    xu_assert(caddr);

    // is expired?
    xu_bool_t ok = xu_false;
    if (caddr->time < g_cache.expired)
    {
        // remove it
        ok = xu_true;

        // trace
        xu_trace_d("del: %s => %{ipaddr}, time: %u, size: %u", (xu_char_t const*)item->name, &caddr->addr, caddr->time,
                   xu_hash_map_size(g_cache.hash));

        // update times
        xu_assert(g_cache.times >= caddr->time);
        g_cache.times -= caddr->time;
    }

    // ok?
    return ok;
}

/* *******************************************************
 * implementation
 * *******************************************************
 */

xu_bool_t xu_dns_cache_init()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // init hash
        if (!g_cache.hash)
            g_cache.hash = xu_hash_map_init(xu_align8(xu_isqrti(XU_DNS_CACHE_MAXN) + 1), xu_element_str(xu_false),
                                            xu_element_mem(sizeof(xu_dns_cache_addr_t), xu_null, xu_null));
        xu_assert_and_check_break(g_cache.hash);

        // ok
        ok = xu_true;

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);

    // failed? exit it
    if (!ok) xu_dns_cache_exit();

    // ok?
    return ok;
}
xu_void_t xu_dns_cache_exit()
{
    // enter
    xu_spinlock_enter(&g_lock);

    // exit hash
    if (g_cache.hash) xu_hash_map_exit(g_cache.hash);
    g_cache.hash = xu_null;

    // exit times
    g_cache.times = 0;

    // exit expired
    g_cache.expired = 0;

    // leave
    xu_spinlock_leave(&g_lock);
}
xu_bool_t xu_dns_cache_get(xu_char_t const* name, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return_val(name && addr, xu_false);

    // trace
    xu_trace_d("get: %s", name);

    // is addr?
    xu_check_return_val(!xu_ipaddr_ip_cstr_set(addr, name, XU_IPADDR_FAMILY_NONE), xu_true);

    // is localhost?
    if (!xu_stricmp(name, "localhost"))
    {
        // save address
        xu_ipaddr_ip_cstr_set(addr, "127.0.0.1", XU_IPADDR_FAMILY_IPV4);

        // ok
        return xu_true;
    }

    // clear address
    xu_ipaddr_clear(addr);

    // enter
    xu_spinlock_enter(&g_lock);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // check
        xu_assert_and_check_break(g_cache.hash);

        // get the host address
        xu_dns_cache_addr_t* caddr = (xu_dns_cache_addr_t*)xu_hash_map_get(g_cache.hash, name);
        xu_check_break(caddr);

        // trace
        xu_trace_d("get: %s => %{ipaddr}, time: %u => %u, size: %u", name, &caddr->addr, caddr->time,
                   xu_dns_cache_now(), xu_hash_map_size(g_cache.hash));

        // update time
        xu_assert_and_check_break(g_cache.times >= caddr->time);
        g_cache.times -= caddr->time;
        caddr->time = xu_dns_cache_now();
        g_cache.times += caddr->time;

        // save address
        xu_ipaddr_copy(addr, &caddr->addr);

        // ok
        ok = xu_true;

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);

    // ok?
    return ok;
}
xu_void_t xu_dns_cache_set(xu_char_t const* name, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return(name && addr);

    // check address
    xu_assert(!xu_ipaddr_ip_is_empty(addr));

    // trace
    xu_trace_d("set: %s => %{ipaddr}", name, addr);

    // init addr
    xu_dns_cache_addr_t caddr;
    caddr.time = xu_dns_cache_now();
    xu_ipaddr_copy(&caddr.addr, addr);

    // enter
    xu_spinlock_enter(&g_lock);

    // done
    do
    {
        // check
        xu_assert_and_check_break(g_cache.hash);

        // remove the expired items if full
        if (xu_hash_map_size(g_cache.hash) >= XU_DNS_CACHE_MAXN)
        {
            // the expired time
            g_cache.expired = ((xu_size_t)(g_cache.times / xu_hash_map_size(g_cache.hash)) + 1);

            // check
            xu_assert_and_check_break(g_cache.expired);

            // trace
            xu_trace_d("expired: %lu", g_cache.expired);

            // remove the expired times
            xu_remove_if(g_cache.hash, xu_dns_cache_clear, xu_null);
        }

        // check
        xu_assert_and_check_break(xu_hash_map_size(g_cache.hash) < XU_DNS_CACHE_MAXN);

        // save addr
        xu_hash_map_insert(g_cache.hash, name, &caddr);

        // update times
        g_cache.times += caddr.time;

        // trace
        xu_trace_d("set: %s => %{ipaddr}, time: %u, size: %u", name, &caddr.addr, caddr.time,
                   xu_hash_map_size(g_cache.hash));

    } while (0);

    // leave
    xu_spinlock_leave(&g_lock);
}
