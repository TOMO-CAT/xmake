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

-- define rule: debug mode
rule("mode.debug", function()
    on_config(function (target)

        -- is debug mode now? xmake f -m debug
        if is_mode("debug") then

            -- enable the debug symbols
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end

            -- disable optimization
            if not target:get("optimize") then
                target:set("optimize", "none")
            end
        end
    end)
end)

-- define rule: release mode
rule("mode.release", function()
    on_config(function (target)

        -- is release mode now? xmake f -m release
        if is_mode("release") then

            -- set the symbols visibility: hidden
            if not target:get("symbols") and target:kind() ~= "shared" then
                target:set("symbols", "hidden")
            end

            -- enable optimization
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end

            -- strip all symbols
            if not target:get("strip") then
                target:set("strip", "all")
            end

            -- enable NDEBUG macros to disables standard-C assertions
            target:add("cxflags", "-DNDEBUG")
        end
    end)
end)

-- define rule: release with debug symbols mode
rule("mode.releasedbg", function()
    on_config(function (target)

        -- is releasedbg mode now? xmake f -m releasedbg
        if is_mode("releasedbg") then

            -- set the symbols visibility: debug
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end

            -- enable optimization
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end

            -- strip all symbols, but it will generate debug symbol file because debug/symbols is setted
            if not target:get("strip") then
                target:set("strip", "all")
            end

            -- enable NDEBUG macros to disables standard-C assertions
            target:add("cxflags", "-DNDEBUG")
        end
    end)
end)

-- define rule: release with minsize mode
rule("mode.minsizerel", function()
    on_config(function (target)

        -- is minsizerel mode now? xmake f -m minsizerel
        if is_mode("minsizerel") then

            -- set the symbols visibility: hidden
            if not target:get("symbols") then
                target:set("symbols", "hidden")
            end

            -- enable optimization
            if not target:get("optimize") then
                target:set("optimize", "smallest")
            end

            -- strip all symbols
            if not target:get("strip") then
                target:set("strip", "all")
            end

            -- enable NDEBUG macros to disables standard-C assertions
            target:add("cxflags", "-DNDEBUG")
        end
    end)
end)

-- define rule: profile mode
rule("mode.profile", function()
    on_config(function (target)

        -- is profile mode now? xmake f -m profile
        if is_mode("profile") then

            -- set the symbols visibility: debug
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end

            -- enable optimization
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end

            -- enable gprof
            target:add("cxflags", "-pg")
            target:add("mxflags", "-pg")
            target:add("ldflags", "-pg")

            -- enable NDEBUG macros to disables standard-C assertions
            target:add("cxflags", "-DNDEBUG")
        end
    end)
end)

-- define rule: coverage mode
rule("mode.coverage", function()
    on_config(function (target)

        -- is coverage mode now? xmake f -m coverage
        if is_mode("coverage") then

            -- enable the debug symbols
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end

            -- disable optimization
            if not target:get("optimize") then
                target:set("optimize", "none")
            end

            -- enable coverage
            target:add("cxflags", "--coverage")
            target:add("mxflags", "--coverage")
            target:add("ldflags", "--coverage")
            target:add("shflags", "--coverage")
        end
    end)
end)

-- define rule: valgrind mode
rule("mode.valgrind", function()
    on_config(function (target)

        -- is valgrind mode now? xmake f -m valgrind
        if is_mode("valgrind") then

            -- enable the debug symbols
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end

            -- enable optimization
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end
        end
    end)
end)
