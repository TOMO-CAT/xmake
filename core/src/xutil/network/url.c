/* *******************************************************
 * trace
 */
#define XU_TRACE_MODULE_NAME "url"
#define XU_TRACE_MODULE_DEBUG (0)

#include "xutil/network/url.h"
#include "xutil/network/impl/http/prefix.h"
#include "xutil/platform/path.h"

/* *******************************************************
 * private implementation
 */

static xu_char_t const* xu_url_parse_args(xu_string_ref_t args, xu_char_t const* p)
{
    // check
    xu_assert_and_check_return_val(args && p, xu_null);

    // clear args
    xu_string_clear(args);

    // skip '?' or ' '
    while (*p && (*p == '?' || xu_isspace(*p)))
        p++;

    // done
    while (*p)
        xu_string_chrcat(args, *p++);

    // end
    return p;
}
static xu_char_t const* xu_url_parse_path(xu_string_ref_t path, xu_char_t const* p, xu_size_t protocol)
{
    // check
    xu_assert_and_check_return_val(path && p, xu_null);

    // clear path
    xu_string_clear(path);

    // skip ' '
    while (*p && xu_isspace(*p))
        p++;

    // append root: '/'
    if (*p != '/' && *p != '\\') xu_string_chrcat(path, '/');

    // done
    xu_char_t ch;
    while ((ch = *p) && ((ch != '?' && ch != '#') || protocol == XU_URL_PROTOCOL_FILE))
    {
        // replace '\\' => '/'
        if (ch == '\\') xu_string_chrcat(path, '/');
        // append character
        else
            xu_string_chrcat(path, ch);

        // next
        p++;
    }

    // end
    return p;
}
static xu_char_t const* xu_url_parse_host(xu_string_ref_t host, xu_char_t const* p)
{
    // check
    xu_assert_and_check_return_val(host && p, xu_null);

    // clear path
    xu_string_clear(host);

    // ipv6? [xxxx:xxxx:...]:port
    if (*p == '[')
    {
        // skip '['
        p++;

        // parse host
        xu_char_t ch;
        while ((ch = *p) && ch != ']')
        {
            // append character
            xu_string_chrcat(host, ch);

            // next
            p++;
        }

        // append ']'
        if (ch == ']') p++;
    }
    else
    {
        // parse host
        xu_char_t ch;
        while ((ch = *p) && !xu_isspace(ch) && ch != ':' && ch != '/' && ch != '\\' && ch != '?' && ch != '&')
        {
            // append character
            xu_string_chrcat(host, ch);

            // next
            p++;
        }
    }

    // end
    return p;
}
static xu_char_t const* xu_url_parse_port(xu_uint16_t* port, xu_char_t const* p)
{
    // check
    xu_assert_and_check_return_val(port && p, xu_null);

    // done
    xu_char_t  data[12] = {0};
    xu_char_t* pb       = data;
    xu_char_t* pe       = data + sizeof(data);
    while (pb < pe && *p && xu_isdigit(*p))
        *pb++ = *p++;
    *port = (xu_uint16_t)xu_s10tou32(data);

    // end
    return p;
}
static xu_char_t const* xu_url_parse_data(xu_string_ref_t data, xu_char_t const* p)
{
    // check
    xu_assert_and_check_return_val(data && p, xu_null);

    // clear path
    xu_string_clear(data);

    // skip '/' and '\\' and ' '
    while (*p && (*p == '/' || *p == '\\' || xu_isspace(*p)))
        p++;

    // done
    xu_char_t ch;
    while ((ch = *p) && !xu_isspace(ch))
    {
        // append data
        xu_string_chrcat(data, ch);

        // next
        p++;
    }

    // end
    return p;
}

/* *******************************************************
 * implementation
 */
xu_bool_t xu_url_init(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // init url
        url->protocol = XU_URL_PROTOCOL_NONE;
        url->is_ssl   = 0;
        xu_ipaddr_clear(&url->addr);
        if (!xu_string_init(&url->host)) break;
        if (!xu_string_init(&url->path)) break;
        if (!xu_string_init(&url->args)) break;
        if (!xu_string_init(&url->cache)) break;

        // ok
        ok = xu_true;

    } while (0);

    // failed? exit it
    if (!ok) xu_url_exit(url);

    // ok
    return ok;
}
xu_bool_t xu_url_init_from_cstr(xu_url_ref_t url, xu_char_t const* cstr)
{
    return xu_url_init(url) && xu_url_cstr_set(url, cstr);
}
xu_void_t xu_url_exit(xu_url_ref_t url)
{
    if (url)
    {
        xu_string_exit(&url->host);
        xu_string_exit(&url->path);
        xu_string_exit(&url->args);
        xu_string_exit(&url->cache);
    }
}
xu_void_t xu_url_clear(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return(url);

    // clear
    url->protocol = XU_URL_PROTOCOL_NONE;
    url->is_ssl   = 0;
    xu_ipaddr_clear(&url->addr);
    xu_string_clear(&url->host);
    xu_string_clear(&url->path);
    xu_string_clear(&url->args);
    xu_string_clear(&url->cache);
}
xu_char_t const* xu_url_cstr(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // exists? return it directly
    if (xu_string_size(&url->cache)) return xu_string_cstr(&url->cache);

    // make
    switch (url->protocol)
    {
    case XU_URL_PROTOCOL_FILE:
    {
        // check
        xu_check_return_val(xu_string_size(&url->path), xu_null);

        // add path
        xu_string_cstrncat(&url->cache, xu_string_cstr(&url->path), xu_string_size(&url->path));
    }
    break;
    case XU_URL_PROTOCOL_SOCK:
    case XU_URL_PROTOCOL_HTTP:
    case XU_URL_PROTOCOL_RTSP:
    {
        // the port
        xu_uint16_t port = xu_ipaddr_port(&url->addr);

        // check
        xu_check_return_val(port && xu_string_size(&url->host), xu_null);

        // add protocol
        if (url->protocol == XU_URL_PROTOCOL_HTTP)
            xu_string_cstrcpy(&url->cache, "http");
        else if (url->protocol == XU_URL_PROTOCOL_SOCK)
            xu_string_cstrcpy(&url->cache, "sock");
        else if (url->protocol == XU_URL_PROTOCOL_RTSP)
            xu_string_cstrcpy(&url->cache, "rtsp");
        else
            xu_assert_and_check_break(0);

        // add ssl
        if (url->is_ssl) xu_string_chrcat(&url->cache, 's');

        // add ://
        xu_string_cstrncat(&url->cache, "://", 3);

        // add host for ipv6
        if (xu_ipaddr_family(&url->addr) == XU_IPADDR_FAMILY_IPV6)
        {
            xu_string_chrcat(&url->cache, '[');
            xu_string_cstrncat(&url->cache, xu_string_cstr(&url->host), xu_string_size(&url->host));
            xu_string_chrcat(&url->cache, ']');
        }
        // add host for ipv4
        else
            xu_string_cstrncat(&url->cache, xu_string_cstr(&url->host), xu_string_size(&url->host));

        // add port
        if ((url->protocol != XU_URL_PROTOCOL_HTTP) || (url->is_ssl && port != XU_HTTP_DEFAULT_PORT_SSL) ||
            (!url->is_ssl && port != XU_HTTP_DEFAULT_PORT))
        {
            xu_string_cstrfcat(&url->cache, ":%u", port);
        }

        // add path
        if (xu_string_size(&url->path))
            xu_string_cstrncat(&url->cache, xu_string_cstr(&url->path), xu_string_size(&url->path));

        // add args
        if (xu_string_size(&url->args))
        {
            xu_string_chrcat(&url->cache, '?');
            xu_string_strcat(&url->cache, &url->args);
        }
    }
    break;
    case XU_URL_PROTOCOL_SQL:
    {
        // add protocol
        xu_string_cstrcpy(&url->cache, "sql://");

        // add host and port
        if (xu_string_size(&url->host))
        {
            // the port
            xu_uint16_t port = xu_ipaddr_port(&url->addr);

            // add host
            xu_string_cstrncat(&url->cache, xu_string_cstr(&url->host), xu_string_size(&url->host));

            // add port
            if (port) xu_string_cstrfcat(&url->cache, ":%u", port);
        }

        // add path
        if (xu_string_size(&url->path))
            xu_string_cstrncat(&url->cache, xu_string_cstr(&url->path), xu_string_size(&url->path));

        // add args
        if (xu_string_size(&url->args))
        {
            xu_string_chrcat(&url->cache, '?');
            xu_string_strcat(&url->cache, &url->args);
        }
    }
    break;
    case XU_URL_PROTOCOL_DATA: break;
    default: break;
    }

    // ok?
    return xu_string_size(&url->cache) ? xu_string_cstr(&url->cache) : xu_null;
}
xu_bool_t xu_url_cstr_set(xu_url_ref_t url, xu_char_t const* cstr)
{
    // check
    xu_assert_and_check_return_val(url && cstr, xu_false);

    // done
    xu_bool_t ok = xu_false;
    do
    {
        // init
        xu_url_clear(url);

        // parse proto
        xu_char_t const* p = cstr;
        xu_char_t        full[XU_PATH_MAXN];
        if (!xu_strnicmp(p, "http://", 7))
        {
            url->protocol = XU_URL_PROTOCOL_HTTP;
            url->is_ssl   = 0;
            p += 7;
        }
        else if (!xu_strnicmp(p, "sock://", 7))
        {
            url->protocol = XU_URL_PROTOCOL_SOCK;
            url->is_ssl   = 0;
            p += 7;
        }
        else if (!xu_strnicmp(p, "file://", 7))
        {
            url->protocol = XU_URL_PROTOCOL_FILE;
            url->is_ssl   = 0;
            p += 7;
        }
        else if (!xu_strnicmp(p, "rtsp://", 7))
        {
            url->protocol = XU_URL_PROTOCOL_RTSP;
            url->is_ssl   = 0;
            p += 7;
        }
        else if (!xu_strnicmp(p, "data://", 7))
        {
            url->protocol = XU_URL_PROTOCOL_DATA;
            url->is_ssl   = 0;
            p += 7;
        }
        else if (!xu_strnicmp(p, "https://", 8))
        {
            url->protocol = XU_URL_PROTOCOL_HTTP;
            url->is_ssl   = 1;
            p += 8;
        }
        else if (!xu_strnicmp(p, "socks://", 8))
        {
            url->protocol = XU_URL_PROTOCOL_SOCK;
            url->is_ssl   = 1;
            p += 8;
        }
        else if (!xu_strnicmp(p, "sql://", 6))
        {
            url->protocol = XU_URL_PROTOCOL_SQL;
            url->is_ssl   = 0;
            p += 6;
        }
        // ./file or /home/file or c:/file or c:\\file ...
        else if ((p = xu_path_absolute(cstr, full, XU_PATH_MAXN)))
        {
            // for unix style path
            if ((*p == '/') || (!xu_strnicmp(p, "file://", 7)))
            {
                url->protocol = XU_URL_PROTOCOL_FILE;
                url->is_ssl   = 0;
                if (*p != '/') p += 7;
            }
            else
                break;
        }
        else
        {
            // trace
            xu_trace_w("[url]: unknown prefix for url: %s", p);
            break;
        }

        // end?
        xu_assert_and_check_break(*p);

        // parse host and port for http or sock or rtsp
        if (url->protocol == XU_URL_PROTOCOL_HTTP || url->protocol == XU_URL_PROTOCOL_SOCK ||
            url->protocol == XU_URL_PROTOCOL_RTSP || url->protocol == XU_URL_PROTOCOL_SQL)
        {
            // parse host
            p = xu_url_parse_host(&url->host, p);
            xu_assert_and_check_break(p);

            // attempt to parse address
            if (xu_string_size(&url->host))
                xu_ipaddr_ip_cstr_set(&url->addr, xu_string_cstr(&url->host), XU_IPADDR_FAMILY_NONE);

            // parse port
            xu_uint16_t port = 0;
            if (*p == ':')
            {
                // parse it
                p = xu_url_parse_port(&port, p + 1);
                xu_assert_and_check_break(p);

                // no port? using the default port
                if (!port) port = url->is_ssl ? XU_HTTP_DEFAULT_PORT_SSL : XU_HTTP_DEFAULT_PORT;
            }
            else if (url->protocol == XU_URL_PROTOCOL_HTTP)
                port = url->is_ssl ? XU_HTTP_DEFAULT_PORT_SSL : XU_HTTP_DEFAULT_PORT;
            else if (url->protocol != XU_URL_PROTOCOL_SQL)
                break;

            // save port
            xu_ipaddr_port_set(&url->addr, port);
        }

        // parse path and args
        if (url->protocol != XU_URL_PROTOCOL_DATA)
        {
            // parse path
            p = xu_url_parse_path(&url->path, p, url->protocol);
            xu_assert_and_check_break(p);

            // find args
            while (*p && *p != '?')
                p++;

            // parse args
            if (*p == '?') xu_url_parse_args(&url->args, p);
        }
        // parse data
        else
            p = xu_url_parse_data(&url->cache, p);

        // ok
        ok = xu_true;

    } while (0);

    // failed? clear the url
    if (!ok) xu_url_clear(url);

    // ok?
    return ok;
}
xu_void_t xu_url_copy(xu_url_ref_t url, xu_url_ref_t copy)
{
    // check
    xu_assert_and_check_return(url && copy);

    // copy it
    url->protocol = copy->protocol;
    url->addr     = copy->addr;
    url->is_ssl   = copy->is_ssl;
    xu_string_strcpy(&url->host, &copy->host);
    xu_string_strcpy(&url->path, &copy->path);
    xu_string_strcpy(&url->args, &copy->args);
    xu_string_strcpy(&url->cache, &copy->cache);
}
xu_bool_t xu_url_ssl(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_false);

    // get ssl
    return url->is_ssl ? xu_true : xu_false;
}
xu_void_t xu_url_ssl_set(xu_url_ref_t url, xu_bool_t is_ssl)
{
    // check
    xu_assert_and_check_return(url);

    // set ssl
    url->is_ssl = is_ssl ? 1 : 0;
}
xu_size_t xu_url_protocol(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, XU_URL_PROTOCOL_NONE);

    // get protocol
    return url->protocol;
}
xu_void_t xu_url_protocol_set(xu_url_ref_t url, xu_size_t protocol)
{
    // check
    xu_assert_and_check_return(url);

    // set protocol
    url->protocol = protocol;
}
xu_char_t const* xu_url_protocol_cstr(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // the protocols
    static xu_char_t const* s_protocols[] = {xu_null, "file", "sock", "http", "data", "rtsp", "sql"};
    xu_assert_and_check_return_val(url->protocol < xu_arrayn(s_protocols), xu_null);

    // ok
    return s_protocols[url->protocol];
}
xu_size_t xu_url_protocol_probe(xu_char_t const* url)
{
    // check
    xu_assert_and_check_return_val(url, XU_URL_PROTOCOL_NONE);

    // init
    xu_char_t        full[XU_PATH_MAXN];
    xu_char_t const* p        = url;
    xu_size_t        protocol = XU_URL_PROTOCOL_NONE;
    if (!xu_strnicmp(p, "http://", 7))
        protocol = XU_URL_PROTOCOL_HTTP;
    else if (!xu_strnicmp(p, "sock://", 7))
        protocol = XU_URL_PROTOCOL_SOCK;
    else if (!xu_strnicmp(p, "file://", 7))
        protocol = XU_URL_PROTOCOL_FILE;
    else if (!xu_strnicmp(p, "data://", 7))
        protocol = XU_URL_PROTOCOL_DATA;
    else if (!xu_strnicmp(p, "https://", 8))
        protocol = XU_URL_PROTOCOL_HTTP;
    else if (!xu_strnicmp(p, "socks://", 8))
        protocol = XU_URL_PROTOCOL_SOCK;
    else if (!xu_strnicmp(p, "rtsp://", 7))
        protocol = XU_URL_PROTOCOL_RTSP;
    else if (!xu_strnicmp(p, "sql://", 6))
        protocol = XU_URL_PROTOCOL_SQL;
    // ./file or /home/file or c:/file or c:\\file ...
    else if ((p = xu_path_absolute(url, full, XU_PATH_MAXN)))
    {
        // for unix style path
        if ((*p == '/') || (!xu_strnicmp(p, "file://", 7))) protocol = XU_URL_PROTOCOL_FILE;
    }
    if (protocol == XU_URL_PROTOCOL_NONE)
    {
        xu_trace_e("unknown protocol for url: %s", url);
    }
    return protocol;
}
xu_uint16_t xu_url_port(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, 0);

    // get port
    return xu_ipaddr_port(&url->addr);
}
xu_void_t xu_url_port_set(xu_url_ref_t url, xu_uint16_t port)
{
    // check
    xu_assert_and_check_return(url);

    // set port
    xu_ipaddr_port_set(&url->addr, port);

    // clear url
    xu_string_clear(&url->cache);
}
xu_char_t const* xu_url_host(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // get host
    return xu_string_size(&url->host) ? xu_string_cstr(&url->host) : xu_null;
}
xu_void_t xu_url_host_set(xu_url_ref_t url, xu_char_t const* host)
{
    // check
    xu_assert_and_check_return(url);

    // clear cache
    xu_string_clear(&url->cache);

    // clear address
    xu_ipaddr_clear(&url->addr);

    // parse host
    xu_url_parse_host(&url->host, host);

    // attempt to parse address
    xu_ipaddr_ip_cstr_set(&url->addr, xu_string_cstr(&url->host), XU_IPADDR_FAMILY_NONE);
}
xu_ipaddr_ref_t xu_url_addr(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // get address
    return &(url->addr);
}
xu_void_t xu_url_addr_set(xu_url_ref_t url, xu_ipaddr_ref_t addr)
{
    // check
    xu_assert_and_check_return(url && addr);

    // changed?
    if (!xu_ipaddr_is_equal(&url->addr, addr))
    {
        // set addr
        url->addr = *addr;

        // set it if the host not exists
        if (!xu_string_size(&url->host))
        {
            // address => host
            xu_char_t        data[XU_IPADDR_CSTR_MAXN];
            xu_char_t const* host = xu_ipaddr_ip_cstr(addr, data, sizeof(data));
            if (host) xu_string_cstrcpy(&url->host, host);

            // clear url
            xu_string_clear(&url->cache);
        }
    }
}
xu_char_t const* xu_url_path(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // get path
    return xu_string_size(&url->path) ? xu_string_cstr(&url->path) : xu_null;
}
xu_void_t xu_url_path_set(xu_url_ref_t url, xu_char_t const* path)
{
    // check
    xu_assert_and_check_return(url);

    // clear cache
    xu_string_clear(&url->cache);

    // parse path
    xu_url_parse_path(&url->path, path, url->protocol);
}
xu_char_t const* xu_url_args(xu_url_ref_t url)
{
    // check
    xu_assert_and_check_return_val(url, xu_null);

    // get args
    return xu_string_size(&url->args) ? xu_string_cstr(&url->args) : xu_null;
}
xu_void_t xu_url_args_set(xu_url_ref_t url, xu_char_t const* args)
{
    // check
    xu_assert_and_check_return(url);

    // clear cache
    xu_string_clear(&url->cache);

    // parse args
    xu_url_parse_args(&url->args, args);
}
