import("lib.detect.find_tool")
import("core.base.semver")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build()
    if ci_is_running() then
        os.run("xmake -r")
    else
        os.run("xmake -r")
    end
    local outdata = os.iorun("xmake")
    if outdata then
        if outdata:find("compiling") or outdata:find("linking") or
            outdata:find("generating") then
            raise("Modules incremental compilation does not work\n%s", outdata)
        end
    end
end

function can_build()
    if is_host("linux") then
        local gcc = find_tool("gcc", {version = true})
        if gcc and gcc.version and semver.compare(gcc.version, "11.0") >= 0 then
            return true
        end
        local clang = find_tool("clang", {version = true})
        if clang and clang.version and semver.compare(clang.version, "14.0") >=
            0 then
            return true
        end
    end
end

function main(t)
    if is_host("linux") then
        local gcc = find_tool("gcc", {version = true})
        if gcc and gcc.version and semver.compare(gcc.version, "11.0") >= 0 then
            cprint("${bright cyan}[test] ====== gcc %s ======${reset}", gcc.version)
            os.exec("xmake f -c --yes")
            _build()
        end
        local clang = find_tool("clang", {version = true})
        if clang and clang.version and semver.compare(clang.version, "14.0") >=
            0 then
            cprint("${bright magenta}[test] ====== clang %s ======${reset}", clang.version)
            os.exec("xmake f --toolchain=clang -c --yes")
            _build()
            -- apt install -y libc++-dev libc++abi-dev
            cprint("${bright yellow}[test] ====== clang %s + c++_shared ======${reset}", clang.version)
            os.exec("xmake f --toolchain=clang --runtimes=c++_shared -c --yes")
            _build()
        end
    end
end
