-- imports
import("privilege.sudo")

local test_build = {}

function test_build:build(argv)

    -- check global config
    os.exec("xmake g -c")

    -- generic?
    os.exec("xmake f -c -D -y")
    os.exec("xmake b -v")
    os.exec("xmake p -D")
    os.exec("xmake install -o $(tmpdir) -a -D")
    os.exec("xmake uninstall --installdir=$(tmpdir) -D")
    os.exec("xmake c -D")
    os.exec("xmake f --mode=debug -D -y")
    os.exec("xmake m -b")
    os.exec("xmake -r -a -D")
    os.exec("xmake m -e buildtest")
    os.exec("xmake m -l")
    os.exec("xmake m buildtest")
    os.exec("xmake m -d buildtest")

    -- test iphoneos?
    if argv and argv.iphoneos then
        if is_host("macosx") then
            os.exec("xmake m package -p iphoneos")
        end
    end
end

function main()
    return test_build
end
