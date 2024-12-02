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
-- @file        xmake.lua
--

toolchain("mingw")
    set_kind("standalone")
    set_homepage("http://www.mingw.org/")
    set_description("Minimalist GNU for Windows")
    set_runtimes("stdc++_static", "stdc++_shared")

    on_check("check")
    on_load(function (toolchain)
        import("core.project.config")

        -- get cross
        local cross
        if toolchain:is_arch("x86_64", "x64") then
            cross = "x86_64-w64-mingw32-"
        elseif toolchain:is_arch("i386", "x86", "i686") then
            cross = "i686-w64-mingw32-"
        elseif toolchain:is_arch("arm64", "aarch64") then
            cross = "aarch64-w64-mingw32-"
        elseif toolchain:is_arch("armv7", "arm.*") then
            cross = "armv7-w64-mingw32-"
        else
            cross = toolchain:cross() or ""
        end

        -- set toolset
        toolchain:add("toolset", "cc", cross .. "gcc")
        toolchain:add("toolset", "cxx", cross .. "g++", cross .. "gcc")
        toolchain:add("toolset", "cpp", cross .. "gcc -E")
        toolchain:add("toolset", "as", cross .. "gcc")
        toolchain:add("toolset", "ld", cross .. "g++", cross .. "gcc")
        toolchain:add("toolset", "sh", cross .. "g++", cross .. "gcc")
        toolchain:add("toolset", "ar", cross .. "ar")
        toolchain:add("toolset", "strip", cross .. "strip")
        toolchain:add("toolset", "ranlib", cross .. "ranlib")
        toolchain:add("toolset", "objcopy", cross .. "objcopy")
        toolchain:add("toolset", "mrc", cross .. "windres")

        -- init flags for architecture
        local archflags = nil
        local arch = toolchain:arch()
        if arch == "x86_64" then archflags = "-m64"
        elseif arch == "i386" then archflags = "-m32"
        end
        if archflags then
            toolchain:add("cxflags", archflags)
            toolchain:add("asflags", archflags)
            toolchain:add("ldflags", archflags)
            toolchain:add("shflags", archflags)
        end
    end)
