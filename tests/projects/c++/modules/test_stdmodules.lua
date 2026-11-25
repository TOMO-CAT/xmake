import("lib.detect.find_tool")
import("core.base.semver")
import("core.base.json")
import("core.tool.toolchain")
import("utils.ci.is_running", {alias = "ci_is_running"})

function _std_modules_available(clang)
    local outdata = try {
        function()
            return os.iorunv(clang.program,
                              {"-std=c++23", "-stdlib=libc++",
                               "--print-library-module-manifest-path"})
        end
    }
    if not outdata then
        return false
    end
    outdata = outdata:trim()
    if outdata == "" or outdata:find("<NOT PRESENT>", 1, true) then
        return false
    end
    if not os.isfile(outdata) then
        return false
    end
    local modules_json = try {function() return json.loadfile(outdata) end}
    local source_path = modules_json and modules_json.modules and
        modules_json.modules[1] and modules_json.modules[1]["source-path"]
    if not source_path then
        return false
    end
    local std_module_directory = path.directory(source_path)
    if not path.is_absolute(std_module_directory) then
        std_module_directory = path.join(path.directory(outdata), std_module_directory)
    end
    return os.isfile(path.join(std_module_directory, "std.cppm")) and
           os.isfile(path.join(std_module_directory, "std.compat.cppm"))
end

function _build()
    if ci_is_running() then
        os.run("xmake -rvD")
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

function main(t)
    if is_host("linux") then -- or is_host("macosx") then
        -- gcc don't support std modules atm
        -- local gcc = find_tool("gcc", {version = true})
        -- if is_host("linux") and gcc and gcc.version and semver.compare(gcc.version, "11.0") >= 0 then
        -- os.exec("xmake f -c --yes")
        -- _build()
        -- end
        local clang = find_tool("clang", {version = true})
        if clang and clang.version and semver.compare(clang.version, "19.0") >=
            0 then
            if not _std_modules_available(clang) then
                print(">>     skipped test_main : libc++ std module sources are not available")
                return
            end
            -- clang don't support libstdc++ std modules atm
            -- os.exec("xmake clean -a")
            -- os.exec("xmake f --toolchain=clang -c --yes")
            -- _build()
            os.exec("xmake clean -a")
            os.exec("xmake f --toolchain=clang --runtimes=c++_shared -c --yes")
            _build()
        end
    end
end
