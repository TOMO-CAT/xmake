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
-- @file        make.lua
--

-- imports
import("core.base.option")
import("core.project.config")
import("lib.detect.find_tool")

-- translate bin path
function _translate_bin_path(bin_path)
    return bin_path
end

-- map compiler flags
function _map_compflags(package, langkind, name, values)
    return compiler.map_flags(langkind, name, values, {target = package})
end

-- map linker flags
function _map_linkflags(package, targetkind, sourcekinds, name, values)
    return linker.map_flags(targetkind, sourcekinds, name, values, {target = package})
end

-- get the build environments
function buildenvs(package)
    local envs = {}
    local cflags   = table.join(table.wrap(package:build_getenv("cxflags")), package:build_getenv("cflags"))
    local cxxflags = table.join(table.wrap(package:build_getenv("cxflags")), package:build_getenv("cxxflags"))
    local asflags  = table.copy(table.wrap(package:config("asflags")))
    local ldflags  = table.copy(table.wrap(package:config("ldflags")))
    local shflags  = table.copy(table.wrap(package:config("shflags")))
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
    if package:is_plat(os.host()) then
        if package:is_plat("linux") and package:is_arch("i386") then
            table.insert(cflags,   "-m32")
            table.insert(cxxflags, "-m32")
            table.insert(asflags,  "-m32")
            table.insert(ldflags,  "-m32")
        end
        envs.CFLAGS    = table.concat(cflags, ' ')
        envs.CXXFLAGS  = table.concat(cxxflags, ' ')
        envs.ASFLAGS   = table.concat(asflags, ' ')
        envs.LDFLAGS   = table.concat(ldflags, ' ')
        envs.SHFLAGS   = table.concat(shflags, ' ')
    else
        envs.CC        = _translate_bin_path(package:build_getenv("cc"))
        envs.CXX       = _translate_bin_path(package:build_getenv("cxx"))
        envs.AS        = _translate_bin_path(package:build_getenv("as"))
        envs.AR        = _translate_bin_path(package:build_getenv("ar"))
        envs.LD        = _translate_bin_path(package:build_getenv("ld"))
        envs.LDSHARED  = _translate_bin_path(package:build_getenv("sh"))
        envs.CPP       = _translate_bin_path(package:build_getenv("cpp"))
        envs.RANLIB    = _translate_bin_path(package:build_getenv("ranlib"))
        envs.CFLAGS    = table.concat(cflags, ' ')
        envs.CXXFLAGS  = table.concat(cxxflags, ' ')
        envs.ASFLAGS   = table.concat(asflags, ' ')
        envs.ARFLAGS   = table.concat(table.wrap(package:build_getenv("arflags")), ' ')
        envs.LDFLAGS   = table.concat(ldflags, ' ')
        envs.SHFLAGS   = table.concat(shflags, ' ')
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
    envs.ACLOCAL_PATH    = path.joinenv(ACLOCAL_PATH)
    envs.PKG_CONFIG_PATH = path.joinenv(PKG_CONFIG_PATH)

    return envs
end

-- do make
function make(package, argv, opt)
    opt = opt or {}
    local program
    local runenvs = opt.envs or buildenvs(package)
    local tool = find_tool("make", {envs = runenvs})
    if tool then
        program = tool.program
    end
    assert(program, "make not found!")
    os.vrunv(program, argv or {}, {envs = runenvs, curdir = opt.curdir})
end

-- build package
function build(package, configs, opt)
    opt = opt or {}

    -- pass configurations
    local njob = opt.jobs or option.get("jobs") or tostring(os.default_njob())
    local argv = {"-j" .. njob}
    if option.get("verbose") then
        table.insert(argv, "VERBOSE=1")
        table.insert(argv, "V=1")
    end
    for name, value in pairs(configs) do
        value = tostring(value):trim()
        if value ~= "" then
            if type(name) == "number" then
                table.insert(argv, value)
            else
                table.insert(argv, name .. "=" .. value)
            end
        end
    end

    -- do build
    make(package, argv, opt)
end

-- install package
function install(package, configs, opt)

    -- do build
    opt = opt or {}
    build(package, configs, opt)

    -- do install
    local argv = {"install"}
    if option.get("verbose") then
        table.insert(argv, "VERBOSE=1")
        table.insert(argv, "V=1")
    end
    make(package, argv, opt)
end
