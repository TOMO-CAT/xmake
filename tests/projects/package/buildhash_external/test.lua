function test_buildhash_with_global_flags(t)
    local projectdir = os.scriptdir()
    local tmpdir = path.join(os.tmpdir(), "xmake_buildhash_external")
    os.mkdir(tmpdir)

    -- get the buildhash of package(foo) when configured with the given extra `xmake config` flags
    local function _buildhash(name, flags)
        local envs = {
            XMAKE_GLOBALDIR = path.join(tmpdir, name, "global"),
            XMAKE_CONFIGDIR = path.join(tmpdir, name, "config")
        }
        os.mkdir(envs.XMAKE_GLOBALDIR)
        os.mkdir(envs.XMAKE_CONFIGDIR)

        local argv = {"f", "-c", "-y"}
        for _, flag in ipairs(flags or {}) do
            table.insert(argv, flag)
        end
        local _, errdata = os.iorunv(os.programfile(), argv, {curdir = projectdir, envs = envs})
        t:require(not errdata or errdata == "")

        local outdata = os.iorunv(os.programfile(), {"l", "print_buildhash.lua"}, {curdir = projectdir, envs = envs})
        local hash
        for _, line in ipairs(table.wrap(outdata and outdata:split("\n", {plain = true}) or {})) do
            hash = line:match("BUILDHASH=(%w+)") or hash
        end
        t:require(hash and #hash > 0)
        return hash
    end

    local default_hash = _buildhash("default", {})
    local lto_hash     = _buildhash("lto", {"--cxxflags=-flto=full"})
    local ldflags_hash = _buildhash("ldflags", {"--ldflags=-s"})

    -- with and without -flto=full lead to different buildhashes
    t:are_not_equal(default_hash, lto_hash)
    -- another global build flag also leads to a different buildhash
    t:are_not_equal(default_hash, ldflags_hash)
    t:are_not_equal(lto_hash, ldflags_hash)
end
