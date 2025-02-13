#include "xutil/unit_test/ut.h"

/* *******************************************************
 * macros
 */
#define XU_DNS_TEST_INVALID_HOST (0)
#define XU_DNS_TEST_MORE_HOST (0)
#define XU_DNS_TEST_HOST_SOME (0)

/* *******************************************************
 * test
 */
static xu_void_t xu_dns_test_done(xu_char_t const* name)
{
    xu_ipaddr_t addr;
    xu_hong_t   time = xu_mclock();
    if (xu_dns_looker_done(name, &addr))
    {
        time = xu_mclock() - time;
        xu_trace_i("lookup: %s => %{ipaddr}, %lld ms", name, &addr, time);
    }
    else
        xu_trace_i("lookup: %s failed", name);
}

/* *******************************************************
 * main
 */
xu_int_t xu_ut_network_dns_main(xu_int_t argc, xu_char_t** argv)
{
    // test the invalid host
#if XU_DNS_TEST_INVALID_HOST
    // add not dns host
    xu_dns_server_add("127.0.0.1");

    // add not ipv4 host
    xu_dns_server_add("localhost");

#endif

    // test the more host
#if XU_DNS_TEST_MORE_HOST
    xu_dns_server_add("2001:470:20::2");
    xu_dns_server_add("fec0:0:0:ffff::1%1");
    xu_dns_server_add("205.252.144.228");
    xu_dns_server_add("208.151.69.65");
    xu_dns_server_add("202.181.202.140");
    xu_dns_server_add("202.181.224.2");
    xu_dns_server_add("202.175.3.8");
    xu_dns_server_add("202.175.3.3");
    xu_dns_server_add("168.95.192.1");
    xu_dns_server_add("168.95.1.1");
    xu_dns_server_add("208.67.222.222");
    xu_dns_server_add("205.171.2.65");
    xu_dns_server_add("193.0.14.129");
    xu_dns_server_add("202.12.27.33");
    xu_dns_server_add("202.216.228.18");
    xu_dns_server_add("209.166.160.132");
    xu_dns_server_add("208.96.10.221");
    xu_dns_server_add("61.144.56.101");
    xu_dns_server_add("202.101.98.55");
    xu_dns_server_add("202.96.128.166");
    xu_dns_server_add("202.96.209.134");
    xu_dns_server_add("221.12.65.228");
#endif

    // sort
    xu_dns_server_sort();

    // dump
#ifdef __xu_debug__
    xu_dns_server_dump();
#endif

#if XU_DNS_TEST_HOST_SOME
    xu_hong_t time = xu_mclock();
    xu_dns_test_done("www.baidu.com");
    xu_dns_test_done("www.google.com");
    xu_dns_test_done("www.google.com.hk");
    xu_dns_test_done("www.csdn.net");
    xu_dns_test_done("www.qq.com");
    xu_dns_test_done("www.youku.com");
    xu_dns_test_done("www.ibm.com");
    xu_dns_test_done("www.sina.com.cn");
    xu_dns_test_done("www.hao123.com");
    xu_dns_test_done("www.sohu.com");
    xu_dns_test_done("www.weibo.com");
    xu_dns_test_done("www.126.com");
    xu_dns_test_done("www.163.com");
    xu_dns_test_done("www.taobao.com");
    xu_dns_test_done("www.microsoft.com");
    xu_dns_test_done("www.qiyi.com");
    xu_dns_test_done("www.xunlei.com");
    xu_dns_test_done("www.360buy.com");
    xu_dns_test_done("www.tudou.com");
    xu_dns_test_done("www.pps.tv");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.zol.com.cn");
    xu_dns_test_done("www.download.com");
    xu_dns_test_done("www.webkit.org");
    xu_dns_test_done("www.douban.com");
    xu_dns_test_done("www.github.com");
    xu_dns_test_done("www.videolan.org");
    xu_dns_test_done("www.net.cn");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.sina.com.cn");
    xu_dns_test_done("www.hao123.com");
    xu_dns_test_done("www.sohu.com");
    xu_dns_test_done("www.weibo.com");
    xu_dns_test_done("www.126.com");
    xu_dns_test_done("www.163.com");
    xu_dns_test_done("www.taobao.com");
    xu_dns_test_done("www.microsoft.com");
    xu_dns_test_done("www.qiyi.com");
    xu_dns_test_done("www.xunlei.com");
    xu_dns_test_done("www.360buy.com");
    xu_dns_test_done("www.tudou.com");
    xu_dns_test_done("www.pps.tv");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.zol.com.cn");
    xu_dns_test_done("www.download.com");
    xu_dns_test_done("www.webkit.org");
    xu_dns_test_done("www.huaxiazi.com");
    xu_dns_test_done("www.facebook.com");
    xu_dns_test_done("www.youtube.com");
    xu_dns_test_done("www.bing.com");
    xu_dns_test_done("www.baidu.com");
    xu_dns_test_done("www.tianya.com");
    xu_dns_test_done("www.adobe.com");
    xu_dns_test_done("web2.qq.com");
    xu_dns_test_done("www.bluehost.com");
    xu_dns_test_done("www.pediy.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("www.gitorious.org");
    xu_dns_test_done("t.qq.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("www.mop.com");
    xu_dns_test_done("www.56.com");
    xu_dns_test_done("www.joy.com.cn");
    xu_dns_test_done("www.xxxxx.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("developer.android.com");
    xu_dns_test_done("developer.apple.com");
    xu_dns_test_done("www.arm.com");
    xu_dns_test_done("www.nginx.com");
    xu_dns_test_done("www.ted.com");
    xu_dns_test_done("www.baidu.com");
    xu_dns_test_done("www.google.com");
    xu_dns_test_done("www.google.com.hk");
    xu_dns_test_done("www.csdn.net");
    xu_dns_test_done("www.qq.com");
    xu_dns_test_done("www.youku.com");
    xu_dns_test_done("www.ibm.com");
    xu_dns_test_done("www.sina.com.cn");
    xu_dns_test_done("www.hao123.com");
    xu_dns_test_done("www.sohu.com");
    xu_dns_test_done("www.weibo.com");
    xu_dns_test_done("www.126.com");
    xu_dns_test_done("www.163.com");
    xu_dns_test_done("www.taobao.com");
    xu_dns_test_done("www.microsoft.com");
    xu_dns_test_done("www.qiyi.com");
    xu_dns_test_done("www.xunlei.com");
    xu_dns_test_done("www.360buy.com");
    xu_dns_test_done("www.tudou.com");
    xu_dns_test_done("www.pps.tv");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.zol.com.cn");
    xu_dns_test_done("www.download.com");
    xu_dns_test_done("www.webkit.org");
    xu_dns_test_done("www.douban.com");
    xu_dns_test_done("www.github.com");
    xu_dns_test_done("www.videolan.org");
    xu_dns_test_done("www.net.cn");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.sina.com.cn");
    xu_dns_test_done("www.hao123.com");
    xu_dns_test_done("www.sohu.com");
    xu_dns_test_done("www.weibo.com");
    xu_dns_test_done("www.126.com");
    xu_dns_test_done("www.163.com");
    xu_dns_test_done("www.taobao.com");
    xu_dns_test_done("www.microsoft.com");
    xu_dns_test_done("www.qiyi.com");
    xu_dns_test_done("www.xunlei.com");
    xu_dns_test_done("www.360buy.com");
    xu_dns_test_done("www.tudou.com");
    xu_dns_test_done("www.pps.tv");
    xu_dns_test_done("www.yahoo.com");
    xu_dns_test_done("www.zol.com.cn");
    xu_dns_test_done("www.download.com");
    xu_dns_test_done("www.webkit.org");
    xu_dns_test_done("www.huaxiazi.com");
    xu_dns_test_done("www.facebook.com");
    xu_dns_test_done("www.youtube.com");
    xu_dns_test_done("www.bing.com");
    xu_dns_test_done("www.baidu.com");
    xu_dns_test_done("www.tianya.com");
    xu_dns_test_done("www.adobe.com");
    xu_dns_test_done("web2.qq.com");
    xu_dns_test_done("www.bluehost.com");
    xu_dns_test_done("www.pediy.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("www.gitorious.org");
    xu_dns_test_done("t.qq.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("www.mop.com");
    xu_dns_test_done("www.56.com");
    xu_dns_test_done("www.joy.com.cn");
    xu_dns_test_done("www.xxxxx.com");
    xu_dns_test_done("www.wordpress.com");
    xu_dns_test_done("developer.android.com");
    xu_dns_test_done("developer.apple.com");
    xu_dns_test_done("www.arm.com");
    xu_dns_test_done("www.nginx.com");
    xu_dns_test_done("www.ted.com");
    xu_dns_test_done("www.mcu-memory.com");
    xu_dns_test_done("mail.126.com");
    xu_dns_test_done("mail.163.com");
    xu_dns_test_done("mail.google.com");
    xu_dns_test_done("mail.qq.com");
    xu_dns_test_done("mail.sina.com");
    xu_dns_test_done("mail.sohu.com");
    xu_dns_test_done("mail.qq.com");
    xu_dns_test_done("www.renren.com");
    xu_dns_test_done("www.cepark.com");
    xu_dns_test_done("www.ifttt.com");
    xu_dns_test_done("www.china-pub.com");
    xu_dns_test_done("www.amazon.com");
    xu_dns_test_done("www.amazon.cn");
    xu_dns_test_done("www.2688.com");
    xu_dns_test_done("www.mtime.com");
    xu_dns_test_done("hi.baidu.com");
    xu_dns_test_done("repo.or.cz");
    xu_dns_test_done("www.ifeng.com");
    xu_dns_test_done("www.sourceforge.com");
    xu_dns_test_done("www.wikipedia.org");
    xu_dns_test_done("baike.baidu.com");
    xu_dns_test_done("www.ted.com");
    xu_dns_test_done("www.ted.com");
    xu_dns_test_done("www.ted.com");
    time = xu_mclock() - time;
    xu_trace_i("[demo]: done %lld ms", time);
#else
    xu_dns_test_done(argv[1]);
#endif

    return 0;
}
