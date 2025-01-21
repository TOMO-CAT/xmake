/* *******************************************************
 * trace
 * *******************************************************
 */
#define XU_TRACE_MODULE_NAME "http_option"
#define XU_TRACE_MODULE_DEBUG (1)

#include "xutil/network/impl/http/option.h"
#include "xutil/network/impl/http/prefix.h"

/* *******************************************************
 * implementation
 * *******************************************************
 */
xu_bool_t xu_http_option_init(xu_http_option_t* option)
{
    // check
    xu_assert_and_check_return_val(option, xu_false);

    // init option using the default value
    option->method   = XU_HTTP_METHOD_GET;
    option->redirect = XU_HTTP_DEFAULT_REDIRECT;
    option->timeout  = XU_HTTP_DEFAULT_TIMEOUT;
    option->version  = 1; // HTTP/1.1
    option->bunzip   = 0;
    option->cookies  = xu_null;

    // init url
    if (!xu_url_init(&option->url)) return xu_false;

    // init post url
    if (!xu_url_init(&option->post_url)) return xu_false;

    // init head data
    if (!xu_buffer_init(&option->head_data)) return xu_false;

    // ok
    return xu_true;
}
xu_void_t xu_http_option_exit(xu_http_option_t* option)
{
    // check
    xu_assert_and_check_return(option);

    // exit url
    xu_url_exit(&option->url);

    // exit post url
    xu_url_exit(&option->post_url);

    // exit head data
    xu_buffer_exit(&option->head_data);

    // clear cookies
    option->cookies = xu_null;
}
xu_bool_t xu_http_option_ctrl(xu_http_option_t* option, xu_size_t code, xu_va_list_t args)
{
    // check
    xu_assert_and_check_return_val(option, xu_false);

    // done
    switch (code)
    {
    case XU_HTTP_OPTION_SET_URL:
    {
        // url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(url, xu_false);

        // set url
        if (xu_url_cstr_set(&option->url, url)) return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_URL:
    {
        // purl
        xu_char_t const** purl = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(purl, xu_false);

        // get url
        xu_char_t const* url = xu_url_cstr(&option->url);
        xu_assert_and_check_return_val(url, xu_false);

        // ok
        *purl = url;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_HOST:
    {
        // host
        xu_char_t const* host = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(host, xu_false);

        // set host
        xu_url_host_set(&option->url, host);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_HOST:
    {
        // phost
        xu_char_t const** phost = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(phost, xu_false);

        // get host
        xu_char_t const* host = xu_url_host(&option->url);
        xu_assert_and_check_return_val(host, xu_false);

        // ok
        *phost = host;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_PORT:
    {
        // port
        xu_size_t port = (xu_size_t)xu_va_arg(args, xu_size_t);
        xu_assert_and_check_return_val(port, xu_false);

        // set port
        xu_url_port_set(&option->url, (xu_uint16_t)port);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_PORT:
    {
        // pport
        xu_size_t* pport = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pport, xu_false);

        // get port
        *pport = xu_url_port(&option->url);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_PATH:
    {
        // path
        xu_char_t const* path = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(path, xu_false);

        // set path
        xu_url_path_set(&option->url, path);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_PATH:
    {
        // ppath
        xu_char_t const** ppath = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(ppath, xu_false);

        // get path
        xu_char_t const* path = xu_url_path(&option->url);
        xu_assert_and_check_return_val(path, xu_false);

        // ok
        *ppath = path;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_METHOD:
    {
        // method
        xu_size_t method = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set method
        option->method = method;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_METHOD:
    {
        // pmethod
        xu_size_t* pmethod = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pmethod, xu_false);

        // get method
        *pmethod = option->method;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_HEAD:
    {
        // key
        xu_char_t const* key = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(key, xu_false);

        // val
        xu_char_t const* val = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(val, xu_false);

        // remove the previous key and value
        xu_bool_t        head_same = xu_false;
        xu_char_t const* head_head = (xu_char_t const*)xu_buffer_data(&option->head_data);
        xu_char_t const* head_data = head_head;
        xu_char_t const* head_tail = head_data + xu_buffer_size(&option->head_data);
        while (head_data < head_tail)
        {
            // the name and data
            xu_char_t const* name = head_data;
            xu_char_t const* data = head_data + xu_strlen(name) + 1;
            xu_char_t const* next = data + xu_strlen(data) + 1;
            xu_check_break(data < head_tail);

            // is this?
            if (!xu_stricmp(name, key))
            {
                // value is different? remove it
                if (xu_stricmp(val, data))
                    xu_buffer_memmovp(&option->head_data, name - head_head, next - head_head);
                else
                    head_same = xu_true;
                break;
            }

            // next
            head_data = next;
        }

        // set head
        if (!head_same)
        {
            xu_buffer_memncat(&option->head_data, (xu_byte_t const*)key, xu_strlen(key) + 1);
            xu_buffer_memncat(&option->head_data, (xu_byte_t const*)val, xu_strlen(val) + 1);
        }

        // ok
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_HEAD:
    {
        // key
        xu_char_t const* key = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(key, xu_false);

        // pval
        xu_char_t const** pval = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(pval, xu_false);

        // find head
        xu_char_t const* head_data = (xu_char_t const*)xu_buffer_data(&option->head_data);
        xu_char_t const* head_tail = head_data + xu_buffer_size(&option->head_data);
        while (head_data < head_tail)
        {
            // the name and data
            xu_char_t const* name = head_data;
            xu_char_t const* data = head_data + xu_strlen(name) + 1;
            xu_check_break(data < head_tail);

            // is this?
            if (!xu_stricmp(name, key))
            {
                // ok
                *pval = data;
                return xu_true;
            }

            // next
            head_data = data + xu_strlen(data) + 1;
        }

        // failed
        return xu_false;
    }
    break;
    case XU_HTTP_OPTION_SET_HEAD_FUNC:
    {
        // head_func
        xu_http_head_func_t head_func = (xu_http_head_func_t)xu_va_arg(args, xu_http_head_func_t);

        // set head_func
        option->head_func = head_func;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_HEAD_FUNC:
    {
        // phead_func
        xu_http_head_func_t* phead_func = (xu_http_head_func_t*)xu_va_arg(args, xu_http_head_func_t*);
        xu_assert_and_check_return_val(phead_func, xu_false);

        // get head_func
        *phead_func = option->head_func;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_HEAD_PRIV:
    {
        // head_priv
        xu_pointer_t head_priv = (xu_pointer_t)xu_va_arg(args, xu_pointer_t);

        // set head_priv
        option->head_priv = head_priv;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_HEAD_PRIV:
    {
        // phead_priv
        xu_pointer_t* phead_priv = (xu_pointer_t*)xu_va_arg(args, xu_pointer_t*);
        xu_assert_and_check_return_val(phead_priv, xu_false);

        // get head_priv
        *phead_priv = option->head_priv;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_RANGE:
    {
        // set range
        option->range.bof = (xu_hize_t)xu_va_arg(args, xu_hize_t);
        option->range.eof = (xu_hize_t)xu_va_arg(args, xu_hize_t);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_RANGE:
    {
        // pbof
        xu_hize_t* pbof = (xu_hize_t*)xu_va_arg(args, xu_hize_t*);
        xu_assert_and_check_return_val(pbof, xu_false);

        // peof
        xu_hize_t* peof = (xu_hize_t*)xu_va_arg(args, xu_hize_t*);
        xu_assert_and_check_return_val(peof, xu_false);

        // ok
        *pbof = option->range.bof;
        *peof = option->range.eof;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_SSL:
    {
        // bssl
        xu_bool_t bssl = (xu_bool_t)xu_va_arg(args, xu_bool_t);

        // set ssl
        xu_url_ssl_set(&option->url, bssl);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_SSL:
    {
        // pssl
        xu_bool_t* pssl = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(pssl, xu_false);

        // get ssl
        *pssl = xu_url_ssl(&option->url);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_TIMEOUT:
    {
        // the timeout
        xu_long_t timeout = (xu_long_t)xu_va_arg(args, xu_long_t);

        // set timeout
        option->timeout = timeout ? timeout : XU_HTTP_DEFAULT_TIMEOUT;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_TIMEOUT:
    {
        // ptimeout
        xu_long_t* ptimeout = (xu_long_t*)xu_va_arg(args, xu_long_t*);
        xu_assert_and_check_return_val(ptimeout, xu_false);

        // get timeout
        *ptimeout = option->timeout;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_COOKIES:
    {
        // set cookies
        option->cookies = (xu_cookies_ref_t)xu_va_arg(args, xu_cookies_ref_t);
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_COOKIES:
    {
        // ptimeout
        xu_cookies_ref_t* pcookies = (xu_cookies_ref_t*)xu_va_arg(args, xu_cookies_ref_t*);
        xu_assert_and_check_return_val(pcookies, xu_false);

        // get cookies
        *pcookies = option->cookies;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_POST_URL:
    {
        // url
        xu_char_t const* url = (xu_char_t const*)xu_va_arg(args, xu_char_t const*);
        xu_assert_and_check_return_val(url, xu_false);

        // clear post data and size
        option->post_data = xu_null;
        option->post_size = 0;

        // set url
        if (xu_url_cstr_set(&option->post_url, url)) return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_POST_URL:
    {
        // purl
        xu_char_t const** purl = (xu_char_t const**)xu_va_arg(args, xu_char_t const**);
        xu_assert_and_check_return_val(purl, xu_false);

        // get url
        xu_char_t const* url = xu_url_cstr(&option->post_url);
        xu_assert_and_check_return_val(url, xu_false);

        // ok
        *purl = url;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_POST_DATA:
    {
        // post data
        xu_byte_t const* data = (xu_byte_t const*)xu_va_arg(args, xu_byte_t const*);

        // post size
        xu_size_t size = (xu_size_t)xu_va_arg(args, xu_size_t);

        // clear post url
        xu_url_clear(&option->post_url);

        // set post data
        option->post_data = data;
        option->post_size = size;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_POST_DATA:
    {
        // pdata and psize
        xu_byte_t const** pdata = (xu_byte_t const**)xu_va_arg(args, xu_byte_t const**);
        xu_size_t*        psize = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pdata && psize, xu_false);

        // get post data and size
        *pdata = option->post_data;
        *psize = option->post_size;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_POST_FUNC:
    {
        // func
        xu_http_post_func_t func = (xu_http_post_func_t)xu_va_arg(args, xu_http_post_func_t);

        // set post func
        option->post_func = func;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_POST_FUNC:
    {
        // pfunc
        xu_http_post_func_t* pfunc = (xu_http_post_func_t*)xu_va_arg(args, xu_http_post_func_t*);
        xu_assert_and_check_return_val(pfunc, xu_false);

        // get post func
        *pfunc = option->post_func;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_POST_PRIV:
    {
        // post priv
        xu_cpointer_t priv = (xu_pointer_t)xu_va_arg(args, xu_pointer_t);

        // set post priv
        option->post_priv = priv;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_POST_PRIV:
    {
        // ppost priv
        xu_cpointer_t* ppriv = (xu_cpointer_t*)xu_va_arg(args, xu_cpointer_t*);
        xu_assert_and_check_return_val(ppriv, xu_false);

        // get post priv
        *ppriv = option->post_priv;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_POST_LRATE:
    {
        // post lrate
        xu_size_t lrate = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set post lrate
        option->post_lrate = lrate;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_POST_LRATE:
    {
        // ppost lrate
        xu_size_t* plrate = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(plrate, xu_false);

        // get post lrate
        *plrate = option->post_lrate;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_AUTO_UNZIP:
    {
        // bunzip
        xu_bool_t bunzip = (xu_bool_t)xu_va_arg(args, xu_bool_t);

        // set bunzip
        option->bunzip = bunzip ? 1 : 0;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_AUTO_UNZIP:
    {
        // pbunzip
        xu_bool_t* pbunzip = (xu_bool_t*)xu_va_arg(args, xu_bool_t*);
        xu_assert_and_check_return_val(pbunzip, xu_false);

        // get bunzip
        *pbunzip = option->bunzip ? xu_true : xu_false;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_REDIRECT:
    {
        // redirect
        xu_size_t redirect = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set redirect
        option->redirect = redirect;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_REDIRECT:
    {
        // predirect
        xu_size_t* predirect = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(predirect, xu_false);

        // get redirect
        *predirect = option->redirect;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_SET_VERSION:
    {
        // version
        xu_size_t version = (xu_size_t)xu_va_arg(args, xu_size_t);

        // set version
        option->version = version;
        return xu_true;
    }
    break;
    case XU_HTTP_OPTION_GET_VERSION:
    {
        // pversion
        xu_size_t* pversion = (xu_size_t*)xu_va_arg(args, xu_size_t*);
        xu_assert_and_check_return_val(pversion, xu_false);

        // get version
        *pversion = option->version;
        return xu_true;
    }
    break;
    default: break;
    }

    // failed
    return xu_false;
}

#ifdef __xu_debug__
xu_void_t xu_http_option_dump(xu_http_option_t* option)
{
    // check
    xu_assert_and_check_return(option);

    // dump option
    xu_trace_i("======================================================================");
    xu_trace_i("option: ");
    xu_trace_i("option: url: %s", xu_url_cstr(&option->url));
    xu_trace_i("option: version: HTTP/1.%1u", option->version);
    xu_trace_i("option: method: %s", xu_http_method_cstr(option->method));
    xu_trace_i("option: redirect: %d", option->redirect);
    xu_trace_i("option: range: %llu-%llu", option->range.bof, option->range.eof);
    xu_trace_i("option: bunzip: %s", option->bunzip ? "true" : "false");

    // dump head
    xu_char_t const* head_data = (xu_char_t const*)xu_buffer_data(&option->head_data);
    xu_char_t const* head_tail = head_data + xu_buffer_size(&option->head_data);
    while (head_data < head_tail)
    {
        // the name and data
        xu_char_t const* name = head_data;
        xu_char_t const* data = head_data + xu_strlen(name) + 1;
        xu_check_break(data < head_tail);

        // trace
        xu_trace_i("option: head: %s: %s", name, data);

        // next
        head_data = data + xu_strlen(data) + 1;
    }

    // dump end
    xu_trace_i("");
}
#endif
