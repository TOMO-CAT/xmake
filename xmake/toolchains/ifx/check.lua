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
-- @file        check.lua
--
-- imports
import("core.base.option")
import("core.project.config")
import("detect.sdks.find_ifxenv")
import("lib.detect.find_tool")

-- check intel on linux
function _check_intel_on_linux(toolchain)
    local ifxenv = toolchain:config("ifxenv")
    if ifxenv then
        return true
    end
    ifxenv = find_ifxenv()
    if ifxenv then
        local ldname = is_host("macosx") and "DYLD_LIBRARY_PATH" or
                           "LD_LIBRARY_PATH"
        local tool = find_tool("ifx", {
            force = true,
            envs = {[ldname] = ifxenv.libdir},
            paths = ifxenv.bindir
        })
        if tool then
            cprint(
                "checking for Intel Fortran Compiler (%s) ... ${color.success}${text.success}",
                toolchain:arch())
            toolchain:config_set("ifxenv", ifxenv)
            toolchain:config_set("bindir", ifxenv.bindir)
            toolchain:configs_save()
            return true
        end
        return true
    end
end

-- main entry
function main(toolchain)
    return _check_intel_on_linux(toolchain)
end
