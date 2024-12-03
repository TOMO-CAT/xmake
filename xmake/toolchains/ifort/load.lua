-- !A cross-toolchain build utility based on Lua
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
-- @file        load.lua
--
-- imports
import("core.base.option")
import("core.project.config")

-- load intel on linux
function _load_intel_on_linux(toolchain)

    -- set toolset
    toolchain:set("toolset", "fc", "ifort")
    toolchain:set("toolset", "fcld", "ifort")
    toolchain:set("toolset", "fcsh", "ifort")
    toolchain:set("toolset", "ar", "ar")

    -- add march flags
    local march
    if toolchain:is_arch("x86_64", "x64") then
        march = "-m64"
    elseif toolchain:is_arch("i386", "x86") then
        march = "-m32"
    end
    if march then
        toolchain:add("fcflags", march)
        toolchain:add("fcldflags", march)
        toolchain:add("fcshflags", march)
    end

    -- get ifort environments
    local ifortenv = toolchain:config("ifortenv")
    if ifortenv then
        local ldname = is_host("macosx") and "DYLD_LIBRARY_PATH" or
                           "LD_LIBRARY_PATH"
        toolchain:add("runenvs", ldname, ifortenv.libdir)
    end
end

-- main entry
function main(toolchain)
    return _load_intel_on_linux(toolchain)
end
