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
toolchain("llvm", function()
    set_kind("standalone")
    set_homepage("https://llvm.org/")
    set_description(
        "A collection of modular and reusable compiler and toolchain technologies")
    set_runtimes("c++_static", "c++_shared", "stdc++_static", "stdc++_shared")

    set_toolset("cc", "clang")
    set_toolset("cxx", "clang", "clang++")
    set_toolset("mxx", "clang", "clang++")
    set_toolset("mm", "clang")
    set_toolset("cpp", "clang -E")
    set_toolset("as", "clang")
    set_toolset("ld", "clang++", "clang")
    set_toolset("sh", "clang++", "clang")
    set_toolset("ar", "llvm-ar")
    set_toolset("strip", "llvm-strip")
    set_toolset("ranlib", "llvm-ranlib")
    set_toolset("objcopy", "llvm-objcopy")
    set_toolset("mrc", "llvm-rc")

    on_check("check")

    on_load(function(toolchain)

        -- add march flags
        local march
        if toolchain:is_arch("x86_64", "x64") then
            march = "-m64"
        elseif toolchain:is_arch("i386", "x86") then
            march = "-m32"
        end
        if march then
            toolchain:add("cxflags", march)
            toolchain:add("mxflags", march)
            toolchain:add("asflags", march)
            toolchain:add("ldflags", march)
            toolchain:add("shflags", march)
        end

        -- init flags for macOS
        if toolchain:is_plat("macosx") then
            local xcode_dir = get_config("xcode")
            local xcode_sdkver = toolchain:config("xcode_sdkver")
            local xcode_sdkdir = nil
            if xcode_dir and xcode_sdkver then
                xcode_sdkdir = xcode_dir ..
                                   "/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX" ..
                                   xcode_sdkver .. ".sdk"
                toolchain:add("cxflags", {"-isysroot", xcode_sdkdir})
                toolchain:add("mxflags", {"-isysroot", xcode_sdkdir})
                toolchain:add("ldflags", {"-isysroot", xcode_sdkdir})
                toolchain:add("shflags", {"-isysroot", xcode_sdkdir})
            else
                -- @see https://github.com/xmake-io/xmake/issues/1179
                local macsdk =
                    "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
                if os.exists(macsdk) then
                    toolchain:add("cxflags", {"-isysroot", macsdk})
                    toolchain:add("mxflags", {"-isysroot", macsdk})
                    toolchain:add("ldflags", {"-isysroot", macsdk})
                    toolchain:add("shflags", {"-isysroot", macsdk})
                end
            end
            toolchain:add("mxflags", "-fobjc-arc")
        end
    end)
end)
