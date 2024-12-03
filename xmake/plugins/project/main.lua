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
-- @file        main.lua
--

-- imports
import("core.base.option")
import("core.base.task")
import("core.project.config")
import("make.makefile")
import("make.xmakefile")
import("cmake.cmakelists")
import("xcode.xcodeproj")
import("ninja.build_ninja")
import("clang.compile_flags")
import("clang.compile_commands")
import("private.utils.statistics")
import("private.service.remote_build.action", {alias = "remote_build_action"})

function makers()
    return {
        make             = makefile.make
    ,   makefile         = makefile.make
    ,   xmakefile        = xmakefile.make
    ,   cmake            = cmakelists.make
    ,   cmakelists       = cmakelists.make
    ,   xcode            = xcodeproj.make
    ,   ninja            = build_ninja.make
    ,   compile_flags    = compile_flags.make
    ,   compile_commands = compile_commands.make
    }
end

-- make project
function _make(kind)
    local maps = makers()
    assert(maps[kind], "the project kind(%s) is not supported!", kind)
    maps[kind](option.get("outputdir"))
end

function main()

    -- do action for remote?
    if remote_build_action.enabled() then
        return remote_build_action()
    end

    -- in project generator?
    os.setenv("XMAKE_IN_PROJECT_GENERATOR", "true")

    -- config it first
    task.run("config")

    -- post statistics
    statistics.post()

    -- make project
    _make(option.get("kind"))

    -- trace
    cprint("${color.success}create ok!")
    os.setenv("XMAKE_IN_PROJECT_GENERATOR", nil)
end
