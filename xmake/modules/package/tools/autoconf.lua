--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, TBOOX Open Source Group.
--
-- @author      ruki
-- @file        autoconf.lua
--

-- imports
import("core.base.option")
import("core.project.config")
import("core.tool.linker")
import("core.tool.compiler")
import("core.tool.toolchain")
import("core.cache.memcache")
import("lib.detect.find_tool")

-- translate paths
function _translate_paths(paths)
    return paths
end

-- map compiler flags
function _map_compflags(package, langkind, name, values)
    return compiler.map_flags(langkind, name, values, {target = package})
end

-- map linker flags
function _map_linkflags(package, targetkind, sourcekinds, name, values)
    return linker.map_flags(targetkind, sourcekinds, name, values, {target = package})
end

-- is cross compilation?
function _is_cross_compilation(package)
    if not package:is_plat(os.subhost()) then
        return true
    end
    if package:is_plat("macosx") and not package:is_arch(os.subarch()) then
        return true
    end
    return false
end

-- get memcache
function _memcache()
    return memcache.cache("package.tools.autoconf")
end

-- has `--with-pic`?
function _has_with_pic(package)
    local has_with_pic = _memcache():get2(tostring(package), "with_pic")
    if has_with_pic == nil then
        local result = try {function() return os.iorunv("./configure", {"--help"}, {shell = true}) end}
        if result and result:find("--with-pic", 1, true) then
            has_with_pic = true
        end
        has_with_pic = has_with_pic or false
        _memcache():set2(tostring(package), "with_pic", has_with_pic)
    end
    return has_with_pic
end

-- get configs
function _get_configs(package, configs)

    -- add prefix
    local configs = configs or {}
    table.insert(configs, "--prefix=" .. _translate_paths(package:installdir()))

    -- add host for cross-complation
    if not configs.host and _is_cross_compilation(package) then
        if package:is_plat("iphoneos", "macosx") then
            local triples =
            {
                arm64  = "aarch64-apple-darwin",
                arm64e = "aarch64-apple-darwin",
                armv7  = "armv7-apple-darwin",
                armv7s = "armv7s-apple-darwin",
                i386   = "i386-apple-darwin",
                x86_64 = "x86_64-apple-darwin"
            }
            table.insert(configs, "--host=" .. (triples[package:arch()] or triples.arm64))
        elseif package:is_plat("android") then
            -- @see https://developer.android.com/ndk/guides/other_build_systems#autoconf
            local triples =
            {
                ["armv5te"]     = "arm-linux-androideabi",  -- deprecated
                ["armv7-a"]     = "arm-linux-androideabi",  -- deprecated
                ["armeabi"]     = "arm-linux-androideabi",  -- removed in ndk r17
                ["armeabi-v7a"] = "arm-linux-androideabi",
                ["arm64-v8a"]   = "aarch64-linux-android",
                i386            = "i686-linux-android",     -- deprecated
                x86             = "i686-linux-android",
                x86_64          = "x86_64-linux-android",
                mips            = "mips-linux-android",     -- removed in ndk r17
                mips64          = "mips64-linux-android"    -- removed in ndk r17
            }
            table.insert(configs, "--host=" .. (triples[package:arch()] or triples["armeabi-v7a"]))
        elseif package:is_plat("cross") and package:targetos() then
            local host = package:arch()
            if package:is_arch("arm64") then
                host = "aarch64"
            elseif package:is_arch("arm.*") then
                host = "arm"
            end
            host = host .. "-" .. package:targetos()
            table.insert(configs, "--host=" .. host)
        end
    end
    if package:is_plat("linux", "bsd") and
        package:config("pic") ~= false and _has_with_pic(package) then
        table.insert(configs, "--with-pic")
    end
    return configs
end

-- get cflags from package deps
function _get_cflags_from_packagedeps(package, opt)
    local result = {}
    for _, depname in ipairs(opt.packagedeps) do
        local dep = type(depname) ~= "string" and depname or package:dep(depname)
        if dep then
            local fetchinfo = dep:fetch({external = false})
            if fetchinfo then
                table.join2(result, _map_compflags(package, "cxx", "define", fetchinfo.defines))
                table.join2(result, _translate_paths(_map_compflags(package, "cxx", "includedir", fetchinfo.includedirs)))
                table.join2(result, _translate_paths(_map_compflags(package, "cxx", "sysincludedir", fetchinfo.sysincludedirs)))
            end
        end
    end
    return result
end

-- get ldflags from package deps
function _get_ldflags_from_packagedeps(package, opt)
    local result = {}
    for _, depname in ipairs(opt.packagedeps) do
        local dep = type(depname) ~= "string" and depname or package:dep(depname)
        if dep then
            local fetchinfo = dep:fetch({external = false})
            if fetchinfo then
                table.join2(result, _translate_paths(_map_linkflags(package, "binary", {"cxx"}, "linkdir", fetchinfo.linkdirs)))
                table.join2(result, _map_linkflags(package, "binary", {"cxx"}, "link", fetchinfo.links))
                table.join2(result, _translate_paths(_map_linkflags(package, "binary", {"cxx"}, "syslink", fetchinfo.syslinks)))
                table.join2(result, _map_linkflags(package, "binary", {"cxx"}, "framework", fetchinfo.frameworks))
            end
        end
    end
    return result
end

-- get the build environments
function buildenvs(package, opt)
    opt = opt or {}
    local envs = {}
    local cross = false
    local cflags, cxxflags, cppflags, asflags, ldflags, shflags, arflags
    if not _is_cross_compilation(package) and not package:config("toolchains") then
        cppflags = {}
        cflags   = table.join(table.wrap(package:config("cxflags")), package:config("cflags"))
        cxxflags = table.join(table.wrap(package:config("cxflags")), package:config("cxxflags"))
        asflags  = table.copy(table.wrap(package:config("asflags")))
        ldflags  = table.copy(table.wrap(package:config("ldflags")))
        shflags  = table.copy(table.wrap(package:config("shflags")))
        if package:is_plat("linux") and package:is_arch("i386") then
            table.insert(cflags,   "-m32")
            table.insert(cxxflags, "-m32")
            table.insert(asflags,  "-m32")
            table.insert(ldflags,  "-m32")
            table.insert(shflags,  "-m32")
        end
        table.join2(cflags,   opt.cflags)
        table.join2(cflags,   opt.cxflags)
        table.join2(cxxflags, opt.cxxflags)
        table.join2(cxxflags, opt.cxflags)
        table.join2(cppflags, opt.cppflags) -- @see https://github.com/xmake-io/xmake/issues/1688
        table.join2(asflags,  opt.asflags)
        table.join2(ldflags,  opt.ldflags)
        table.join2(shflags,  opt.shflags)
        table.join2(cflags,   _get_cflags_from_packagedeps(package, opt))
        table.join2(cxxflags, _get_cflags_from_packagedeps(package, opt))
        table.join2(cppflags, _get_cflags_from_packagedeps(package, opt))
        table.join2(ldflags,  _get_ldflags_from_packagedeps(package, opt))
        table.join2(shflags,  _get_ldflags_from_packagedeps(package, opt))
    else
        cross = true
        cppflags = {}
        cflags   = table.join(table.wrap(package:build_getenv("cxflags")), package:build_getenv("cflags"))
        cxxflags = table.join(table.wrap(package:build_getenv("cxflags")), package:build_getenv("cxxflags"))
        asflags  = table.copy(table.wrap(package:build_getenv("asflags")))
        ldflags  = table.copy(table.wrap(package:build_getenv("ldflags")))
        shflags  = table.copy(table.wrap(package:build_getenv("shflags")))
        arflags  = table.copy(table.wrap(package:build_getenv("arflags")))
        local defines        = package:build_getenv("defines")
        local includedirs    = package:build_getenv("includedirs")
        local sysincludedirs = package:build_getenv("sysincludedirs")
        local links          = package:build_getenv("links")
        local syslinks       = package:build_getenv("syslinks")
        local linkdirs       = package:build_getenv("linkdirs")
        table.join2(cflags,   opt.cflags)
        table.join2(cflags,   opt.cxflags)
        table.join2(cxxflags, opt.cxxflags)
        table.join2(cxxflags, opt.cxflags)
        table.join2(cppflags, opt.cppflags) -- @see https://github.com/xmake-io/xmake/issues/1688
        table.join2(asflags,  opt.asflags)
        table.join2(ldflags,  opt.ldflags)
        table.join2(shflags,  opt.shflags)
        table.join2(arflags,  opt.arflags)
        table.join2(cflags,   _get_cflags_from_packagedeps(package, opt))
        table.join2(cxxflags, _get_cflags_from_packagedeps(package, opt))
        table.join2(cppflags, _get_cflags_from_packagedeps(package, opt))
        table.join2(ldflags,  _get_ldflags_from_packagedeps(package, opt))
        table.join2(cflags,   _map_compflags(package, "c", "define", defines))
        table.join2(cflags,   _map_compflags(package, "c", "includedir", includedirs))
        table.join2(cflags,   _map_compflags(package, "c", "sysincludedir", sysincludedirs))
        table.join2(asflags,  _map_compflags(package, "as", "define", defines))
        table.join2(asflags,  _map_compflags(package, "as", "includedir", includedirs))
        table.join2(asflags,  _map_compflags(package, "as", "sysincludedir", sysincludedirs))
        table.join2(cxxflags, _map_compflags(package, "cxx", "define", defines))
        table.join2(cxxflags, _map_compflags(package, "cxx", "includedir", includedirs))
        table.join2(cxxflags, _map_compflags(package, "cxx", "sysincludedir", sysincludedirs))
        table.join2(ldflags,  _map_linkflags(package, "binary", {"cxx"}, "link", links))
        table.join2(ldflags,  _map_linkflags(package, "binary", {"cxx"}, "syslink", syslinks))
        table.join2(ldflags,  _map_linkflags(package, "binary", {"cxx"}, "linkdir", linkdirs))
        table.join2(shflags,  _map_linkflags(package, "shared", {"cxx"}, "link", links))
        table.join2(shflags,  _map_linkflags(package, "shared", {"cxx"}, "syslink", syslinks))
        table.join2(shflags,  _map_linkflags(package, "shared", {"cxx"}, "linkdir", linkdirs))
        envs.CC        = package:build_getenv("cc")
        envs.AS        = package:build_getenv("as")
        envs.AR        = package:build_getenv("ar")
        envs.LD        = package:build_getenv("ld")
        envs.LDSHARED  = package:build_getenv("sh")
        envs.CPP       = package:build_getenv("cpp")
        envs.RANLIB    = package:build_getenv("ranlib")
    end
    if package:is_plat("linux", "bsd") and
        package:config("pic") ~= false and not _has_with_pic(package) then
        table.insert(cflags, "-fPIC")
        table.insert(cxxflags, "-fPIC")
    end
    if package:config("lto") then
        table.join2(cflags, package:_generate_lto_configs("cc").cflags)
        table.join2(cxxflags, package:_generate_lto_configs("cxx").cxxflags)
        table.join2(ldflags, package:_generate_lto_configs().ldflags)
    end
    local runtimes = package:runtimes()
    if runtimes then
        local fake_target = {is_shared = function(_) return false end,
                             sourcekinds = function(_) return "cxx" end}
        table.join2(cxxflags, _map_compflags(fake_target, "cxx", "runtime", runtimes))
        table.join2(ldflags, _map_linkflags(fake_target, "binary", {"cxx"}, "runtime", runtimes))
        fake_target = {is_shared = function(_) return true end,
                       sourcekinds = function(_) return "cxx" end}
        table.join2(shflags, _map_linkflags(fake_target, "shared", {"cxx"}, "runtime", runtimes))
    end
    if package:config("asan") then
        table.join2(cflags, package:_generate_sanitizer_configs("address", "cc").cflags)
        table.join2(cxxflags, package:_generate_sanitizer_configs("address", "cxx").cxxflags)
        table.join2(ldflags, package:_generate_sanitizer_configs("address").ldflags)
        table.join2(shflags, package:_generate_sanitizer_configs("address").shflags)
    end
    if cflags then
        envs.CFLAGS    = table.concat(_translate_paths(cflags), ' ')
    end
    if cxxflags then
        envs.CXXFLAGS  = table.concat(_translate_paths(cxxflags), ' ')
    end
    if cppflags then
        envs.CPPFLAGS  = table.concat(_translate_paths(cppflags), ' ')
    end
    if asflags then
        envs.ASFLAGS   = table.concat(_translate_paths(asflags), ' ')
    end
    if arflags then
        envs.ARFLAGS   = table.concat(_translate_paths(arflags), ' ')
    end
    if ldflags or shflags then
        -- autoconf does not use SHFLAGS
        envs.LDFLAGS   = table.concat(_translate_paths(table.join(ldflags or {}, shflags)), ' ')
    end

    -- cross-compilation? pass the full build environments
    if cross then
        if package:is_plat("macosx") then
            -- force to apply shflags on macosx https://gmplib.org/manual/Known-Build-Problems
            envs.CC = envs.CC .. " -arch " .. package:arch()
        end
        if package:is_plat("cross") or package:has_tool("ar", "ar", "emar") then
            -- only for cross-toolchain
            envs.CXX = package:build_getenv("cxx")
            if not envs.ARFLAGS or envs.ARFLAGS == "" then
                envs.ARFLAGS = "-cr"
            end
        end

        -- we should use ld as linker
        --
        -- @see
        -- https://github.com/xmake-io/xmake-repo/pull/1043
        -- https://github.com/libexpat/libexpat/issues/312
        -- https://github.com/xmake-io/xmake/issues/2195
        local ld = envs.LD
        if ld and package:has_tool("ld", "clang", "clangxx", "gcc", "gxx") then
            local dir = path.directory(ld)
            local name = path.filename(ld)
            name = name:gsub("clang%+%+$", "ld")
            name = name:gsub("clang%+%+%-%d+", "ld")
            name = name:gsub("clang$", "ld")
            name = name:gsub("clang%-%d+", "ld")
            name = name:gsub("gcc$", "ld")
            name = name:gsub("gcc-%d+", "ld")
            name = name:gsub("g%+%+$", "ld")
            name = name:gsub("g%+%+%-%d+", "ld")
            envs.LD = dir and path.join(dir, name) or name
        end
        -- we need use clang++ as cxx, autoconf will use it as linker
        -- https://github.com/xmake-io/xmake/issues/2170
        local cxx = envs.CXX
        if cxx and package:has_tool("cxx", "clang", "gcc") then
            local dir = path.directory(cxx)
            local name = path.filename(cxx)
            name = name:gsub("clang$", "clang++")
            name = name:gsub("clang%-", "clang++-")
            name = name:gsub("gcc$", "g++")
            name = name:gsub("gcc%-", "g++-")
            envs.CXX = dir and path.join(dir, name) or name
        end
    end
    local ACLOCAL_PATH = {}
    local PKG_CONFIG_PATH = {}
    for _, dep in ipairs(package:librarydeps({private = true})) do
        local pkgconfig = path.join(dep:installdir(), "lib", "pkgconfig")
        if os.isdir(pkgconfig) then
            table.insert(PKG_CONFIG_PATH, pkgconfig)
        end
        pkgconfig = path.join(dep:installdir(), "share", "pkgconfig")
        if os.isdir(pkgconfig) then
            table.insert(PKG_CONFIG_PATH, pkgconfig)
        end
    end
    -- some binary packages contain it too. e.g. libtool
    for _, dep in ipairs(package:orderdeps()) do
        local aclocal = path.join(dep:installdir(), "share", "aclocal")
        if os.isdir(aclocal) then
            table.insert(ACLOCAL_PATH, aclocal)
        end
    end
    envs.ACLOCAL_PATH = path.joinenv(ACLOCAL_PATH)
    envs.PKG_CONFIG_PATH = path.joinenv(PKG_CONFIG_PATH)
    return envs
end

-- get the autogen environments
function autogen_envs(package, opt)
    opt = opt or {}
    local envs = {NOCONFIGURE = "yes"}
    local ACLOCAL_PATH = {}
    local PKG_CONFIG_PATH = {}
    for _, dep in ipairs(package:librarydeps({private = true})) do
        local pkgconfig = path.join(dep:installdir(), "lib", "pkgconfig")
        if os.isdir(pkgconfig) then
            table.insert(PKG_CONFIG_PATH, pkgconfig)
        end
        pkgconfig = path.join(dep:installdir(), "share", "pkgconfig")
        if os.isdir(pkgconfig) then
            table.insert(PKG_CONFIG_PATH, pkgconfig)
        end
    end
    -- some binary packages contain it too. e.g. libtool
    for _, dep in ipairs(package:orderdeps()) do
        local aclocal = path.join(dep:installdir(), "share", "aclocal")
        if os.isdir(aclocal) then
            table.insert(ACLOCAL_PATH, aclocal)
        end
    end
    envs.ACLOCAL_PATH    = path.joinenv(ACLOCAL_PATH)
    envs.PKG_CONFIG_PATH = path.joinenv(PKG_CONFIG_PATH)
    return envs
end

-- configure package
function configure(package, configs, opt)

    -- init options
    opt = opt or {}

    -- generate configure file
    if not os.isfile("configure") then
        if os.isfile("autogen.sh") then
            os.vrunv("./autogen.sh", {}, {shell = true, envs = autogen_envs(package, opt)})
        elseif os.isfile("configure.ac") or os.isfile("configure.in") then
            local autoreconf = find_tool("autoreconf")
            assert(autoreconf, "autoreconf not found!")
            os.vrunv(autoreconf.program, {"--install", "--symlink"}, {shell = true, envs = autogen_envs(package, opt)})
        end
    end

    -- get envs
    local envs = opt.envs or buildenvs(package, opt)

    -- pass configurations
    local argv = {}
    for name, value in pairs(_get_configs(package, configs)) do
        value = tostring(value):trim()
        if value ~= "" then
            if type(name) == "number" then
                table.insert(argv, value)
            else
                table.insert(argv, "--" .. name .. "=" .. value)
            end
        end
    end

    -- do configure
    os.vrunv("./configure", argv, {shell = true, envs = envs})
end

-- do make
function make(package, argv, opt)
    opt = opt or {}
    local program
    local tool = find_tool("make")
    if tool then
        program = tool.program
    end
    assert(program, "make not found!")
    os.vrunv(program, argv)
end

-- build package
function build(package, configs, opt)

    -- do configure
    configure(package, configs, opt)

    -- do make and install
    opt = opt or {}
    local njob = opt.jobs or option.get("jobs") or tostring(os.default_njob())
    local argv = {"-j" .. njob}
    if option.get("diagnosis") then
        table.insert(argv, "V=1")
    end
    if opt.makeconfigs then
        for name, value in pairs(opt.makeconfigs) do
            value = tostring(value):trim()
            if value ~= "" then
                if type(name) == "number" then
                    table.insert(argv, value)
                else
                    table.insert(argv, name .. "=" .. value)
                end
            end
        end
    end
    make(package, argv, opt)
end

-- install package
function install(package, configs, opt)

    -- do build
    opt = opt or {}
    build(package, configs, opt)

    -- do install
    local argv = {"install"}
    if option.get("diagnosis") then
        table.insert(argv, "V=1")
    end
    if opt.makeconfigs then
        for name, value in pairs(opt.makeconfigs) do
            value = tostring(value):trim()
            if value ~= "" then
                if type(name) == "number" then
                    table.insert(argv, value)
                else
                    table.insert(argv, name .. "=" .. value)
                end
            end
        end
    end
    make(package, argv, opt)
end
