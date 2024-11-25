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
-- @file        gcc_ar.lua
--

inherit("ar")
import("lib.detect.find_file")

-- replace gcc-ar
function _replace_gcc_ar(program, name)
    local dir = path.directory(program)
    local filename = path.filename(program)
    filename = filename:gsub("gcc%-ar", name)
    if dir and dir ~= "." then
        program = path.join(dir, filename)
    else
        program = filename
    end
    return program
end

-- get liblto_plugin.so path for gcc
function _get_gcc_liblto_plugin_path(self, program)
    local plugin_path = _g.LTO_PLUGIN_PATH
    if plugin_path == nil then
        local gcc = _replace_gcc_ar(program, "gcc")
        local outdata = try { function() return os.iorunv(gcc, {"-print-prog-name=lto-wrapper"}) end }
        if outdata then
            local lto_plugindir = path.directory(outdata:trim())
            if os.isdir(lto_plugindir) then
                plugin_path = find_file("liblto_plugin.so", lto_plugindir)
            end
        end
        plugin_path = plugin_path or false
        _g.LTO_PLUGIN_PATH = plugin_path
    end
    return plugin_path or nil
end

-- link the library file
function link(self, objectfiles, targetkind, targetfile, flags, opt)
    opt = opt or {}
    os.mkdir(path.directory(targetfile))

    -- @note remove the previous archived file first to force recreating a new file
    os.tryrm(targetfile)

    -- generate link arguments
    local program, argv = linkargv(self, objectfiles, targetkind, targetfile, flags, opt)

    -- link it
    os.runv(program, argv, {envs = self:runenvs(), shell = opt.shell})
end
