-- !A cross-platform build utility based on Lua
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
toolchain("emcc", function()

    set_homepage("http://emscripten.org")
    set_description("A toolchain for compiling to asm.js and WebAssembly")

    set_kind("standalone")

    set_toolset("cc", "emcc")
    set_toolset("cxx", "emcc", "em++")
    set_toolset("ld", "em++", "emcc")
    set_toolset("sh", "em++", "emcc")
    set_toolset("ar", "emar")
    set_toolset("as", "emcc")
    set_toolset("ranlib", "emranlib")

    on_check(function(toolchain)
        import("lib.detect.find_tool")
        import("detect.sdks.find_emsdk")
        for _, package in ipairs(toolchain:packages()) do
            local installdir = package:installdir()
            if installdir and os.isdir(installdir) then
                local emsdk = find_emsdk(installdir)
                if emsdk then
                    toolchain:config_set("bindir", emsdk.emscripten)
                    toolchain:config_set("sdkdir", emsdk.sdkdir)
                    toolchain:configs_save()
                    return emsdk
                end
            end
        end
        return find_tool("emcc")
    end)

    on_load(function(toolchain)
        toolchain:add("cxflags", "")
        toolchain:add("asflags", "")
        toolchain:add("ldflags", "")
        toolchain:add("shflags", "")
        for _, package in ipairs(toolchain:packages()) do
            local envs = package:get("envs")
            if envs then
                for _, name in ipairs({
                    "EMSDK", "EMSDK_NODE", "EMSDK_PYTHON", "JAVA_HOME"
                }) do
                    local values = envs[name]
                    if values then
                        toolchain:add("runenvs", name, table.unwrap(values))
                    end
                end
            end
        end
    end)
end)
