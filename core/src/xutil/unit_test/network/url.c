#include "xutil/unit_test/ut.h"

/* *******************************************************
 * test
 */
static xu_void_t xu_url_test_set(xu_char_t const* url)
{
    xu_url_t u;
    xu_url_init(&u);
    if (xu_url_cstr_set(&u, url))
    {
        xu_trace_i("=================================================");
        xu_trace_i("irl: %s", url);
        xu_trace_i("orl: %s", xu_url_cstr(&u));
        xu_trace_i("poto: %s", xu_url_protocol_cstr(&u));
        xu_trace_i("port: %u", xu_url_port(&u));
        xu_trace_i("host: %s", xu_url_host(&u));
        xu_trace_i("path: %s", xu_url_path(&u));
        xu_trace_i("args: %s", xu_url_args(&u));
    }
    else
        xu_trace_i("invalid url: %s", url);
    xu_url_exit(&u);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_url_main(xu_int_t argc, xu_char_t** argv)
{
    xu_url_test_set("data://VGhlIFRyZWFzdXJlIEJveCBMaWJyYXJ5Cgo=");
    xu_url_test_set("file:///home/cat/file.txt");
    xu_url_test_set("sock://localhost:8080");
    xu_url_test_set("sock://localhost:8080/cat/file.txt");
    xu_url_test_set("rtsp://localhost:8080");
    xu_url_test_set("rtsp://localhost:8080/cat/file.txt");
    xu_url_test_set("http://localhost");
    xu_url_test_set("http://localhost:80");
    xu_url_test_set("http://localhost:8080");
    xu_url_test_set("http://localhost/cat/file.txt");
    xu_url_test_set("http://localhost:8080/cat/file.txt");
    xu_url_test_set("http://[::1]:8080/cat/file.txt");
    xu_url_test_set("http://[fe80::7a31:c1ff:fecf:e8ae%en0]:8080/cat/file.txt");
    xu_url_test_set("http://[fe80::7a31:c1ff:fecf:e8ae%7]:8080/cat/file.txt");

    xu_url_test_set("socks://localhost:8080");
    xu_url_test_set("socks://localhost:8080/cat/file.txt");
    xu_url_test_set("https://localhost");
    xu_url_test_set("https://localhost:80");
    xu_url_test_set("https://localhost:8080");
    xu_url_test_set("https://localhost/cat/file.txt");
    xu_url_test_set("https://localhost:443/cat/file.txt");
    xu_url_test_set("https://localhost:8080/cat/file.txt");
    xu_url_test_set("https://localhost:8080/cat/file.txt/#/hello");
    xu_url_test_set("https://github.githubassets.com/#start-of-content");

    xu_url_test_set("file:///home/cat/file.txt?");
    xu_url_test_set("sock://localhost:8080?udp=");
    xu_url_test_set("sock://localhost:8080/cat/file.txt?proto=tcp");
    xu_url_test_set("http://localhost?=");
    xu_url_test_set("http://127.0.0.1:8080?ssl=true");
    xu_url_test_set("http://localhost/cat/file.txt?arg0=11111&arg1=222222");
    xu_url_test_set("http://localhost:8080/cat/file.txt?xxxx&sds");
    xu_url_test_set("http://vcode.baidu.com/"
                    "genimage?"
                    "00140254475301668D95A071A0C29E7F10FD51CA7F1548CF49B02D47F821146A1F7A4154B649D30D2E4FE5B24AD645EAA0"
                    "D957938A0E838D854084A77B3589F7E0E6784E9919C2178A5978F8CA51E878E1DB3B84C7D2BE5E4BC145811FEC7DBE6E17"
                    "DB25931A00F0A31E45A7C476FF8209BD9DDD6824D89D8E0EC7260FE53F4B412FC67D161DE6FEFC3F884E079170A1A80856"
                    "29B84AED10352DCAA946D80EDCBB04A664936CAB0DDC585110D11D2747C27260EF6CD44648");
    xu_url_test_set("http://中文.中文");

    xu_url_test_set("FILE:///HOME/CAT/FILE.TXT?");
    xu_url_test_set("SOCK://LOCALHOST:8080?UDP=");
    xu_url_test_set("SOCK://LOCALHOST:8080/CAT/FILE.TXT?PROTO=TCP");
    xu_url_test_set("HTTP://LOCALHOST?=");
    xu_url_test_set("HTTP://127.0.0.1:8080?SSL=TRUE");
    xu_url_test_set("HTTP://LOCALHOST/CAT/FILE.TXT?ARG0=11111&ARG1=222222");
    xu_url_test_set("HTTP://LOCALHOST:8080/CAT/FILE.TXT?XXXX&SDS");

    xu_url_test_set("sql://localhost/?type=mysql&user=xxxx&pwd=xxxx");
    xu_url_test_set("sql://localhost:3306/?type=mysql&user=xxxx&pwd=xxxx&database=xxxx");
    xu_url_test_set("Sql:///home/file.sqlite3?type=sqlite3");

    xu_url_test_set("C:/HOME/CAT/FILE.TXT");
    xu_url_test_set("d:/home/cat/file.txt");
    xu_url_test_set("d:\\home\\cat\\file.txt");

    xu_url_test_set("/home/cat/file.txt");
    xu_url_test_set("cat/file.txt");
    xu_url_test_set("./cat/file.txt");
    xu_url_test_set("../cat/file.txt");
    xu_url_test_set("../../cat/file.txt");
    xu_url_test_set("..\\..\\cat\\file.txt");
    xu_url_test_set("../../cat/../file.txt");

    return 0;
}
