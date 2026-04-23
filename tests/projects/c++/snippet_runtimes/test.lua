import("lib.detect.find_tool")
import("core.base.semver")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build()
    if ci_is_running() then
        assert(os.iorun("xmake -r"))
    else
        assert(os.iorun("xmake -r"))
    end
end

function main(t)
    local clang = find_tool("clang")
    if clang then
        -- libc++ 头文件 / 运行库
        -- sudo apt install -y libc++-dev libc++abi-dev
        os.exec("xmake f -c --toolchain=clang --runtimes=c++_shared --yes")
        _build()
    end
end
