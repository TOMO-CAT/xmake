function main(t)
    -- install libtool failed
    --
    -- https://github.com/conan-io/conan-center-index/issues/25251
    --
    -- => installing libtool .. (1/make) error: @programdir/core/sandbox/modules/os.lua:273: /usr/bin/make  install-recursive
    -- make[1]: Entering directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    -- Making install in .
    -- make[2]: Entering directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    -- make[3]: Entering directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    --  /usr/bin/mkdir -p '/home/cat/.xmake/packages/l/libtool/2.4.7/f710939d070045c099f49e266ac2733e/bin'
    --  /usr/bin/install -c libtool '/home/cat/.xmake/packages/l/libtool/2.4.7/f710939d070045c099f49e266ac2733e/bin'
    --  /usr/bin/mkdir -p '/home/cat/.xmake/packages/l/libtool/2.4.7/f710939d070045c099f49e266ac2733e/lib'
    --  /bin/bash ./libtool   --mode=install /usr/bin/install -c   libltdl/libltdl.la '/home/cat/.xmake/packages/l/libtool/2.4.7/f710939d070045c099f49e266ac2733e/lib'
    -- libtool: install: /usr/bin/install -c libltdl/.libs/libltdl.so.7.3.2 /home/cat/.xmake/packages/l/libtool/2.4.7/f710939d070045c099f49e266ac2733e/lib/libltdl.so.7.3.2
    -- /usr/bin/install: cannot stat 'libltdl/.libs/libltdl.so.7.3.2': No such file or directory
    -- make[3]: *** [Makefile:1081: install-libLTLIBRARIES] Error 1
    -- make[3]: Leaving directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    -- make[2]: *** [Makefile:1966: install-am] Error 2
    -- make[2]: Leaving directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    -- make[1]: *** [Makefile:1648: install-recursive] Error 1
    -- make[1]: Leaving directory '/home/cat/.xmake/cache/packages/2410/l/libtool/2.4.7/source'
    -- make: *** [Makefile:1959: install] Error 2


    -- freebsd ci is slower
    if is_host("bsd") then
        return
    end

    -- only for x86/x64, because it will take too long time on ci with arm/mips
    if os.subarch():startswith("x") or os.subarch() == "i386" then
        t:build()
    end
end
