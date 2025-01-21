/* *******************************************************
 * trace
 * *******************************************************
 */

#define XU_TRACE_MODULE_NAME "cookies"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/cookies.h"
#include "xutil/algorithm/remove_if.h"
#include "xutil/container/hash_set.h"
#include "xutil/libc/stdlib/stdlib.h"
#include "xutil/memory/string_pool.h"
#include "xutil/network/impl/http/data.h"
#include "xutil/platform/cache_time.h"
#include "xutil/platform/path.h"
#include "xutil/utils/singleton.h"

/* *******************************************************
 * type
 * *******************************************************
 */

// the cookies entry type
typedef struct __xu_cookies_entry_t
{
    // the domain
    xu_char_t const* domain;

    // the path
    xu_char_t const* path;

    // the name
    xu_char_t const* name;

    // the value
    xu_char_t const* value;

    // the expires
    xu_time_t expires;

    // the max-age, default: 1 and storage: 0
    xu_uint32_t maxage : 30;

    // storage cookies to file? remove it immediately if maxage == 0 and storage: 0
    xu_uint32_t storage : 1;

    // is secure?
    xu_uint32_t secure : 1;

} xu_cookies_entry_t, *xu_cookies_entry_ref_t;

// the cookies type
typedef struct __xu_cookies_t
{
    // the lock
    xu_spinlock_t lock;

    // the string element
    xu_element_t string_element;

    // the string pool
    xu_string_pool_ref_t string_pool;

    // the cookie pool, key: "domain+path+name"
    xu_hash_set_ref_t cookie_pool;

} xu_cookies_t;

/* *******************************************************
 * private implementation
 * *******************************************************
 */

static xu_bool_t xu_cookies_get_domain_and_path_from_url(xu_char_t const* url, xu_char_t* pdomain, xu_size_t ndomain,
                                                         xu_char_t* ppath, xu_size_t npath, xu_bool_t* psecure)
{
    // check
    xu_assert_and_check_return_val(url && pdomain && ndomain && ppath && npath && psecure, xu_false);

    // get url pointer
    xu_char_t const* p = url;
    xu_char_t const* e = url + xu_strlen(url);
    xu_assert_and_check_return_val(p < e, xu_false);

    // skip "http://"
    if (p + 7 < e && !xu_strnicmp(p, "http://", 7))
    {
        p += 7;
    }
    // skip "https://"
    else if (p + 8 < e && !xu_strnicmp(p, "https://", 8))
    {
        p += 8;
        *psecure = xu_true;
    }

    // skip www
    if (p + 3 < e && !xu_strnicmp(p, "www", 3)) p += 3;

    // skip .
    if (p + 1 < e && *p == '.') p++;

    // get domain
    xu_char_t* pb = (xu_char_t*)pdomain;
    xu_char_t* pe = (xu_char_t*)pdomain + ndomain - 1;
    while (p < e && pb < pe && *p && *p != '/' && *p != ':')
        *pb++ = *p++;
    *pb = '\0';

    // trace
    xu_trace_d("domain: %s", pdomain);

    // skip port
    if (*p && *p == ':')
        for (p++; p < e && *p && *p != '/'; p++)
            ;

    // get path
    pb = ppath;
    pe = ppath + npath - 1;
    while (p < e && pb < pe && *p && *p != '?')
        *pb++ = *p++;
    *pb = '\0';

    // no path?
    if (!*ppath)
    {
        *ppath++ = '/';
        *ppath   = '\0';
    }

    // trace
    xu_trace_d("path: %s", ppath);

    // ok?
    return *pdomain ? xu_true : xu_false;
}
static xu_bool_t xu_cookies_is_child_domain(xu_char_t const* parent, xu_char_t const* child)
{
    // check
    xu_assert_and_check_return_val(parent && child, xu_false);

    // init
    xu_char_t const* pb = parent;
    xu_char_t const* cb = child;
    xu_size_t        pn = xu_strlen(pb);
    xu_size_t        cn = xu_strlen(cb);
    xu_assert_and_check_return_val(pn && cn, xu_false);

    // is child?
    xu_size_t        n  = 0;
    xu_char_t const* pe = pb + pn - 1;
    xu_char_t const* ce = cb + cn - 1;
    for (; pe >= pb && ce >= cb && *pe == *ce; pe--, ce--)
    {
        if (*ce == '.') n++;
    }

    // ok?
    return (pe < pb && n >= 1 && (ce < cb || *ce == '.')) ? xu_true : xu_false;
}
static xu_bool_t xu_cookies_is_child_path(xu_char_t const* parent, xu_char_t const* child)
{
    // check
    xu_assert_and_check_return_val(parent && child, xu_false);

    // parent is root?
    if (parent[0] == '/' && !parent[1]) return xu_true;

    // is child?
    xu_char_t const* p = parent;
    xu_char_t const* c = child;
    xu_size_t        n = xu_strlen(parent);
    while (n-- && *p && *c && *p++ == *c++)
        ;

    // ok?
    return (!*p && (!*c || *c == '/')) ? xu_true : xu_false;
}
static xu_void_t xu_cookies_entry_exit(xu_cookies_t* cookies, xu_cookies_entry_ref_t entry)
{
    // check
    xu_assert_and_check_return(cookies && entry);

    // exit domain
    if (entry->domain) xu_string_pool_remove(cookies->string_pool, entry->domain);
    entry->domain = xu_null;

    // exit path
    if (entry->path) xu_string_pool_remove(cookies->string_pool, entry->path);
    entry->path = xu_null;

    // exit name
    if (entry->name) xu_string_pool_remove(cookies->string_pool, entry->name);
    entry->name = xu_null;

    // exit value
    if (entry->value) xu_string_pool_remove(cookies->string_pool, entry->value);
    entry->value = xu_null;
}
static xu_void_t xu_cookies_entry_free(xu_element_ref_t element, xu_pointer_t buff)
{
    // check
    xu_cookies_entry_ref_t entry = (xu_cookies_entry_ref_t)buff;
    xu_assert_and_check_return(element && entry);

    // the cookies
    xu_cookies_t* cookies = (xu_cookies_t*)element->priv;
    xu_assert_and_check_return(cookies && cookies->string_pool);

    // exit it
    xu_cookies_entry_exit(cookies, entry);
}
static xu_size_t xu_cookies_entry_hash(xu_element_ref_t element, xu_cpointer_t data, xu_size_t mask, xu_size_t index)
{
    // check
    xu_cookies_entry_ref_t entry = (xu_cookies_entry_ref_t)data;
    xu_assert_and_check_return_val(element && entry && entry->domain, 0);

    // the cookies
    xu_cookies_t* cookies = (xu_cookies_t*)element->priv;
    xu_assert_and_check_return_val(cookies && cookies->string_element.hash, 0);

    // compute the three hash values
    xu_size_t v0 = cookies->string_element.hash(&cookies->string_element, entry->domain, mask, index);
    xu_size_t v1 = entry->path ? cookies->string_element.hash(&cookies->string_element, entry->path, mask, index) : 0;
    xu_size_t v2 = entry->name ? cookies->string_element.hash(&cookies->string_element, entry->name, mask, index) : 0;

    // the hash value
    return (v0 ^ v1 ^ v2) & mask;
}
static xu_long_t xu_cookies_entry_comp(xu_element_ref_t element, xu_cpointer_t ldata, xu_cpointer_t rdata)
{
    // check
    xu_cookies_entry_ref_t lentry = (xu_cookies_entry_ref_t)ldata;
    xu_cookies_entry_ref_t rentry = (xu_cookies_entry_ref_t)rdata;
    xu_assert_and_check_return_val(lentry && lentry->domain, 0);
    xu_assert_and_check_return_val(rentry && rentry->domain, 0);

    // compare domain
    xu_long_t ok = xu_strcmp(lentry->domain, rentry->domain);
    xu_check_return_val(!ok, ok);

    // compare domain
    ok = xu_strcmp(lentry->path ? lentry->path : "", rentry->path ? rentry->path : "");
    xu_check_return_val(!ok, ok);

    // compare name
    return xu_strcmp(lentry->name ? lentry->name : "", rentry->name ? rentry->name : "");
}
static xu_bool_t xu_cookies_entry_init(xu_cookies_t* cookies, xu_cookies_entry_ref_t entry, xu_char_t const* domain,
                                       xu_char_t const* path, xu_bool_t secure, xu_char_t const* value)
{
    // check
    xu_assert_and_check_return_val(cookies && cookies->string_pool && entry && value, xu_false);

    // init maxage: -1
    entry->maxage  = 1;
    entry->storage = 0;

    // done
    xu_char_t const* p          = value;
    xu_char_t const* b          = xu_null;
    xu_char_t const* v          = xu_null;
    xu_char_t        data[4096] = {0};
    while (1)
    {
        // key=value; or value;
        if (!*p || *p == ';')
        {
            // end?
            xu_check_break(b);

            // trace
            xu_trace_d("entry: %s => %s", b ? b : "", v ? v : "");

            // done value
            if (!xu_strnicmp(b, "expires", 7))
            {
                // must have value
                xu_assert_and_check_return_val(v, xu_false);

                // make expires time
                entry->expires = xu_http_date_from_cstr(v, p - v);
            }
            else if (!xu_strnicmp(b, "max-age", 7))
            {
                // must have value
                xu_assert_and_check_return_val(v, xu_false);

                // the maxage
                xu_long_t maxage = xu_stoi32(v);

                // storage to file?
                entry->storage = maxage > 0 ? 1 : 0;

                // save maxage
                entry->maxage = xu_abs(maxage);
            }
            else if (!xu_strnicmp(b, "domain", 6))
            {
                // must have value
                xu_assert_and_check_return_val(v, xu_false);

                // save value
                xu_assert_and_check_return_val(p - v < sizeof(data), xu_false);
                if (v < p)
                {
                    xu_strncpy(data, v, p - v);
                    data[p - v]   = '\0';
                    entry->domain = xu_string_pool_insert(cookies->string_pool, data[0] == '.' ? data + 1 : data);
                }
            }
            else if (!xu_strnicmp(b, "path", 4))
            {
                // must have value
                xu_assert_and_check_return_val(v, xu_false);

                // save value
                xu_assert_and_check_return_val(p - v < sizeof(data), xu_false);
                if (v < p)
                {
                    xu_strncpy(data, v, p - v);
                    data[p - v] = '\0';
                    entry->path = xu_string_pool_insert(cookies->string_pool, data);
                }
            }
            else if (!xu_strnicmp(b, "version", 7))
            {
                // must have value
                xu_assert_and_check_return_val(v, xu_false);
            }
            else if (!xu_strnicmp(b, "secure", 6))
                entry->secure = 1;
            // ignore it
            else if (!xu_strnicmp(b, "HttpOnly", 8))
                ;
            // key=value
            else if (v)
            {
                // must have value
                xu_assert_and_check_return_val(v > b, xu_false);

                // save name
                xu_assert_and_check_return_val(v - b - 1 < sizeof(data), xu_false);
                xu_strncpy(data, b, v - b - 1);
                data[v - b - 1] = '\0';
                entry->name     = xu_string_pool_insert(cookies->string_pool, data);
                xu_assert_and_check_return_val(entry->name, xu_false);

                // save value
                xu_assert_and_check_return_val(p - v < sizeof(data), xu_false);
                if (v < p)
                {
                    xu_strncpy(data, v, p - v);
                    data[p - v]  = '\0';
                    entry->value = xu_string_pool_insert(cookies->string_pool, data);
                    xu_assert_and_check_return_val(entry->value, xu_false);
                }

                // trace
                xu_trace_d("set %s=%s", entry->name, entry->value ? entry->value : "");
            }

            // next key-value pair
            if (*p)
            {
                b = xu_null;
                v = xu_null;
                p++;
            }
            // end
            else
                break;
        }
        // skip space for name
        else if (!b && !v && *p == ' ')
            p++;
        // point to name
        else if (!b && *p)
            b = p++;
        // point to value
        else if (!v && *p == '=')
            v = ++p;
        // next
        else
            p++;
    }

    // domain not exists? using the given domain
    if (!entry->domain && domain)
    {
        // the domain size
        xu_size_t n = xu_strlen(domain);

        // skip www
        if (n > 3 && !xu_strnicmp(domain, "www", 3))
        {
            domain += 3;
            n -= 3;
        }

        // skip .
        if (n && *domain == '.') domain++;

        // save domain
        entry->domain = xu_string_pool_insert(cookies->string_pool, domain);
    }
    if (!entry->domain)
    {
        // trace
        xu_trace_e("no domain for value: %s", value);
        return xu_false;
    }

    // path not exists? using the given path
    if (!entry->path) entry->path = xu_string_pool_insert(cookies->string_pool, path ? path : "/");
    xu_assert_and_check_return_val(entry->path, xu_false);

    // no secure? using the given secure value
    if (!entry->secure && secure) entry->secure = 1;

    // ok
    return xu_true;
}
static xu_bool_t xu_cookies_entry_walk(xu_iterator_ref_t iterator, xu_cpointer_t item, xu_cpointer_t value)
{
    // check
    xu_value_t* tuple = (xu_value_t*)value;
    xu_assert(item && tuple);

    // the entry
    xu_cookies_entry_ref_t entry = (xu_cookies_entry_ref_t)item;
    xu_assert(entry && entry->domain);

    // the domain
    xu_char_t const* domain = tuple[0].cstr;

    // the path
    xu_char_t const* path = tuple[1].cstr;

    // the secure
    xu_size_t secure = tuple[2].ul;

    // the value of key
    xu_string_ref_t kvalue = (xu_string_ref_t)tuple[3].ptr;
    xu_assert(kvalue);

    // expired?
    if (entry->expires && xu_cache_time() >= entry->expires)
    {
        // trace
        xu_trace_d("expired: %s%s%s: %s = %s", entry->secure ? "https://" : "http://", entry->domain,
                   entry->path ? entry->path : "", entry->name ? entry->name : "", entry->value ? entry->value : "");

        // remove it
        return xu_true;
    }

    // this cookies is at domain/path?
    if (xu_cookies_is_child_domain(entry->domain, domain) && entry->path && entry->name &&
        xu_cookies_is_child_path(entry->path, path) && entry->secure == secure)
    {
        // append "key=value; "
        xu_string_cstrfcat(kvalue, "%s=%s; ", entry->name, entry->value ? entry->value : "");
    }

    // continue
    return xu_false;
}

/* *******************************************************
 * instance implementation
 * *******************************************************
 */
static xu_handle_t xu_cookies_instance_init(xu_cpointer_t* ppriv) { return (xu_handle_t)xu_cookies_init(); }
static xu_void_t   xu_cookies_instance_exit(xu_handle_t cookies, xu_cpointer_t priv)
{
    // dump it
#ifdef __xu_debug__
    xu_cookies_dump((xu_cookies_ref_t)cookies);
#endif

    // exit it
    xu_cookies_exit((xu_cookies_ref_t)cookies);
}

/* *******************************************************
 * interface implementation
 * *******************************************************
 */
xu_cookies_ref_t xu_cookies()
{
    return (xu_cookies_ref_t)xu_singleton_instance(XU_SINGLETON_TYPE_COOKIES, xu_cookies_instance_init,
                                                   xu_cookies_instance_exit, xu_null, xu_null);
}
xu_cookies_ref_t xu_cookies_init()
{
    // done
    xu_bool_t     ok      = xu_false;
    xu_cookies_t* cookies = xu_null;
    do
    {
        // make cookies
        cookies = xu_malloc0_type(xu_cookies_t);
        xu_assert_and_check_break(cookies);

        // init lock
        if (!xu_spinlock_init(&cookies->lock)) break;

        // init string pool
        cookies->string_pool = xu_string_pool_init(xu_true);
        xu_assert_and_check_break(cookies->string_pool);

        // init cookie pool
        xu_element_t element = xu_element_mem(sizeof(xu_cookies_entry_t), xu_cookies_entry_free, cookies);
        element.hash         = xu_cookies_entry_hash;
        element.comp         = xu_cookies_entry_comp;
        cookies->cookie_pool = xu_hash_set_init(XU_HASH_SET_BUCKET_SIZE_MICRO, element);
        xu_assert_and_check_break(cookies->cookie_pool);

        // init string element
        cookies->string_element = xu_element_str(xu_true);

        // register lock profiler
#ifdef XU_LOCK_PROFILER_ENABLE
        xu_lock_profiler_register(xu_lock_profiler(), (xu_pointer_t)&cookies->lock, XU_TRACE_MODULE_NAME);
#endif

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit cookies
        if (cookies) xu_cookies_exit((xu_cookies_ref_t)cookies);
        cookies = xu_null;
    }

    // ok?
    return (xu_cookies_ref_t)cookies;
}
xu_void_t xu_cookies_exit(xu_cookies_ref_t self)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return(cookies);

    // enter
    xu_spinlock_enter(&cookies->lock);

    // exit cookie pool
    if (cookies->cookie_pool) xu_hash_set_exit(cookies->cookie_pool);
    cookies->cookie_pool = xu_null;

    // exit string pool
    if (cookies->string_pool) xu_string_pool_exit(cookies->string_pool);
    cookies->string_pool = xu_null;

    // leave
    xu_spinlock_leave(&cookies->lock);

    // exit lock
    xu_spinlock_exit(&cookies->lock);

    // exit it
    xu_free(cookies);
}
xu_void_t xu_cookies_clear(xu_cookies_ref_t self)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return(cookies);

    // enter
    xu_spinlock_enter(&cookies->lock);

    // clear cookie pool
    if (cookies->cookie_pool) xu_hash_set_clear(cookies->cookie_pool);

    // clear string pool
    if (cookies->string_pool) xu_string_pool_clear(cookies->string_pool);

    // leave
    xu_spinlock_leave(&cookies->lock);
}
xu_bool_t xu_cookies_set(xu_cookies_ref_t self, xu_char_t const* domain, xu_char_t const* path, xu_bool_t secure,
                         xu_char_t const* value)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return_val(cookies, xu_false);

    // enter
    xu_spinlock_enter(&cookies->lock);

    // done
    xu_bool_t          ok    = xu_false;
    xu_cookies_entry_t entry = {0};
    do
    {
        // check
        xu_assert_and_check_break(cookies->string_pool && cookies->cookie_pool);

        // init entry
        if (!xu_cookies_entry_init(cookies, &entry, domain, path, secure, value)) break;

        // maxage is zero? remove it
        if (!entry.maxage && !entry.storage)
        {
            // remove it
            xu_hash_set_remove(cookies->cookie_pool, &entry);

            // exit it
            xu_cookies_entry_exit(cookies, &entry);
        }
        // set entry
        else
            xu_hash_set_insert(cookies->cookie_pool, &entry);

        // storage to file?
        if (entry.storage)
        {
            // TODO
            xu_trace1_w("not supports storaging cookies to file now!");
        }

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        xu_cookies_entry_exit(cookies, &entry);
    }

    // leave
    xu_spinlock_leave(&cookies->lock);

    // ok?
    return ok;
}
xu_bool_t xu_cookies_set_from_url(xu_cookies_ref_t self, xu_char_t const* url, xu_char_t const* value)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return_val(cookies, xu_false);

    // get domain and path from the given url
    xu_bool_t secure             = xu_false;
    xu_char_t domain[256]        = {0};
    xu_char_t path[XU_PATH_MAXN] = {0};
    if (xu_cookies_get_domain_and_path_from_url(url, domain, sizeof(domain) - 1, path, sizeof(path) - 1, &secure))
    {
        // trace
        xu_trace_d("domain: %s, path: %s, secure: %s", domain, path, secure ? "ok" : "no");

        // set it with domain and path
        return xu_cookies_set(self, domain, path, secure, value);
    }

    // try to set it without domain and path
    return xu_cookies_set(self, xu_null, xu_null, xu_false, value);
}
xu_char_t const* xu_cookies_get(xu_cookies_ref_t self, xu_char_t const* domain, xu_char_t const* path, xu_bool_t secure,
                                xu_string_ref_t value)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return_val(cookies && domain && value, xu_null);

    // clear value first
    xu_string_clear(value);

    // enter
    xu_spinlock_enter(&cookies->lock);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // check
        xu_assert_and_check_break(cookies->string_pool && cookies->cookie_pool);

        // no path? using the root path
        if (!path || !path[0]) path = "/";

        // skip '.'
        if (*domain == '.') domain++;

        // spak the cached time
        xu_cache_time_spak();

        // get the matched values
        xu_value_t tuple[4];
        tuple[0].cstr = domain;
        tuple[1].cstr = path;
        tuple[2].ul   = secure ? 1 : 0;
        tuple[3].ptr  = value;
        xu_remove_if(cookies->cookie_pool, xu_cookies_entry_walk, tuple);

        // ok
        ok = xu_true;

    } while (0);

    // failed?
    if (!ok) xu_string_clear(value);

    // leave
    xu_spinlock_leave(&cookies->lock);

    // ok?
    return xu_string_size(value) ? xu_string_cstr(value) : xu_null;
}
xu_char_t const* xu_cookies_get_from_url(xu_cookies_ref_t self, xu_char_t const* url, xu_string_ref_t value)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return_val(cookies && value, xu_null);

    // get domain and path from the given url
    xu_bool_t secure             = xu_false;
    xu_char_t domain[256]        = {0};
    xu_char_t path[XU_PATH_MAXN] = {0};
    if (!xu_cookies_get_domain_and_path_from_url(url, domain, sizeof(domain) - 1, path, sizeof(path) - 1, &secure))
        return xu_null;

    // trace
    xu_trace_d("domain: %s, path: %s, secure: %s", domain, path, secure ? "ok" : "no");

    // get it from domain and path
    return xu_cookies_get(self, domain, path, secure, value);
}
#ifdef __xu_debug__
xu_void_t xu_cookies_dump(xu_cookies_ref_t self)
{
    // check
    xu_cookies_t* cookies = (xu_cookies_t*)self;
    xu_assert_and_check_return(cookies && cookies->cookie_pool);

    // enter
    xu_spinlock_enter(&cookies->lock);

    // dump
    xu_trace_i("");
    xu_trace_i("cookie: size: %lu", xu_hash_set_size(cookies->cookie_pool));
    xu_for_all_if(xu_cookies_entry_ref_t, entry, cookies->cookie_pool, entry)
    {
        // the entry
        xu_assert_and_check_continue(entry->domain);

        // the date
        xu_tm_t date = {0};
        xu_gmtime(entry->expires, &date);

        // trace
        xu_trace_i("%s%s%s: %s = %s, expires: %04ld-%02ld-%02ld %02ld:%02ld:%02ld GMT, week: %d",
                   entry->secure ? "https://" : "http://", entry->domain, entry->path ? entry->path : "",
                   entry->name ? entry->name : "", entry->value ? entry->value : "", date.year, date.month, date.mday,
                   date.hour, date.minute, date.second, date.week);
    }

    // leave
    xu_spinlock_leave(&cookies->lock);
}
#endif
