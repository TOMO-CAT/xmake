import("lib.detect.find_tool")
import("core.base.semver")
import("detect.sdks.find_vstudio")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _build()
    if ci_is_running() then
        os.run("xmake -rvD")
    else
        os.run("xmake -r")
    end
    local outdata = os.iorun("xmake")
    if outdata then
        if outdata:find("compiling") or outdata:find("linking") or outdata:find("generating") then
            raise("Modules incremental compilation does not work\n%s", outdata)
        end
    end
end

function main(t)
    if is_subhost("windows") then
        local clang = find_tool("clang", { version = true })
        if clang and clang.version and semver.compare(clang.version, "15.0") >= 0 then
            -- clang headerunit are bugged
            -- os.exec("xmake f --toolchain=clang --policies=build.c++.clang.fallbackscanner -c --yes")
            -- _build()
            -- if semver.compare(clang.version, "17.0") >= 0 then
            --     os.exec("xmake clean -a")
            --     -- clang-scan-deps dependency detection doesn't support header units atm
            --     os.exec("xmake f --toolchain=clang --policies=build.c++.clang.fallbackscanner --runtimes=c++_shared -c --yes")
            --     _build()
            -- end
        end

        local vs = find_vstudio()
        if vs and vs["2022"] then
            os.exec("xmake clean -a")
            os.exec("xmake f -c --yes")
            _build()
        end
    elseif is_subhost("msys") then
        -- on windows, mingw modulemapper doesn't handle headeunit path correctly, but it's working with mingw on macOS / Linux
        -- os.exec("xmake f -c -p mingw --yes")
        -- _build()
    elseif is_host("linux") then
        -- only support for clang
        --
        -- local gcc = find_tool("gcc", {version = true})
        -- if gcc and gcc.version and semver.compare(gcc.version, "11.0") >= 0 then
        --     -- gcc dependency detection doesn't support header units atm
        --     os.exec("xmake f --policies=build.c++.gcc.fallbackscanner -c --yes")
        --     _build()
        -- end
        local clang = find_tool("clang", {version = true})
        if clang and clang.version then
            if semver.compare(clang.version, "15.0") >= 0 then
                os.exec("xmake clean -a")
                -- clang-scan-deps dependency detection doesn't support header units atm
                os.exec("xmake f --toolchain=clang --policies=build.c++.clang.fallbackscanner -c --yes")
                _build()
            end
            -- libc++ headerunit are bugged
            -- if semver.compare(clang.version, "17.0") >= 0 then
            --     os.exec("xmake clean -a")
            --     -- clang-scan-deps dependency detection doesn't support header units atm
            --     os.exec("xmake f --toolchain=clang --policies=build.c++.clang.fallbackscanner --runtimes=c++_shared -c --yes")
            --     _build()
            -- end
        end
    end
end
