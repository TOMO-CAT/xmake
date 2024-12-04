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
-- @file        ccache.lua
--

-- imports
import("core.project.config")
import("lib.detect.find_tool")
import("core.cache.memcache")
import("core.project.project")

-- get ccache tool
function _ccache()
    local ccache = _g.ccache
    if ccache == nil and config.get("ccache") then
        ccache = find_tool("ccache")
        _g.ccache = ccache or false
    end
    return ccache or nil
end

-- get memcache
function _memcache()
    local cache = _g.memcache
    if not cache then
        cache = memcache.cache("ccache")
        _g.memcache = cache
    end
    return cache
end

-- is enabled?
function is_enabled(target)
    local key = tostring(target or "all")
    local result = _memcache():get2("enabled", key)
    if result == nil then
        -- check if ccache exist?
        if result == nil and not exists() then
            result = false
        end
        -- target may be option instance
        if result == nil and target and target.policy then
            result = target:policy("build.ccache")
        end
        if result == nil and os.isfile(os.projectfile()) then
            local policy = project.policy("build.ccache")
            if policy ~= nil then
                result = policy
            end
        end
        if result == nil then
            result = config.get("ccache")
        end
        result = result or false
        _memcache():set2("enabled", key)
    end
    return result
end

-- exists ccache?
function exists()
    return _ccache() ~= nil
end

-- uses ccache to wrap the program and arguments
--
-- e.g. ccache program argv
--
function cmdargv(program, argv)

    -- uses ccache?
    local ccache = _ccache()
    if ccache then

        -- parse the filename and arguments, e.g. "xcrun -sdk macosx clang"
        if not os.isexec(program) then
            argv = table.join(program:split("%s"), argv)
        else
            table.insert(argv, 1, program)
        end
        return ccache.program, argv
    end
    return program, argv
end
