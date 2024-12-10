-- main entry
function main(t)

    -- freebsd ci is slower
    if is_host("bsd") then
        return
    end

    -- only for x86/x64, because it will take too long time on ci with arm/mips
    if os.subarch():startswith("x") or os.subarch() == "i386" then
        os.exec("xmake g -c")
        os.exec("xmake f -c -D -y")

        -- -y for package with "package.install_always" policy
        -- @see https://github.com/TOMO-CAT/xmake/issues/108
        os.exec("xmake -y")
        os.exec("xmake p -D -y")

        os.exec("xmake install -o $(tmpdir) -a -D")
        os.exec("xmake uninstall --installdir=$(tmpdir) -D")
        os.exec("xmake c -D")
        os.exec("xmake f --mode=debug -D -y")
        os.exec("xmake m -b")
        os.exec("xmake -r -a -D -y")
        os.exec("xmake m -e buildtest")
        os.exec("xmake m -l")
        os.exec("xmake m -y buildtest")
        os.exec("xmake m -d buildtest")
    end
end
