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

-- define rule: protobuf.cpp
rule("protobuf.cpp")
    add_deps("c++")
    set_extensions(".proto")

    -- use rule:on_config instead of rule:on_load to fix the situation that we add proto files at
    -- target's on_load function, otherwise we cannot get the value of `target:sourcebatches()`
    --
    -- eg.
    --
    -- target("foo.proto", function()
    --     set_kind("static")
    --     add_packages("protobuf")
    --     add_packages("bat.proto")
    --     add_rules("protobuf.cpp")
    --     on_load(function(target)
    --         local bar_proto_installdir = target:pkg("bat.proto"):installdir()
    --         target:add("files", "foo/**.proto", {
    --             proto_public = true,
    --             proto_rootdir = ".",
    --             extra_flags = { "-I" .. bar_proto_installdir }
    --         })
    --     end)
    --     set_policy('build.fence', true)
    -- end)
    after_load(function(target)
        import("proto").load(target, "cxx")
    end)

    -- before_buildcmd_file is override by before_build_files at build action
    -- and it has some problems such as loss of dependent information
    -- @see https://github.com/TOMO-CAT/xmake/issues/30
    -- so we disable `before_buildcmd_file` and `on_buildcmd_file` for now
    --
    -- but we need `before_buildcmd_file` and `on_buildcmd_file` to generate compile_commands.json
    -- so we use `before_build_files` and `on_build_file` to override them
    before_buildcmd_file(function(target, batchcmds, sourcefile_proto, opt)
        import("proto").buildcmd_pfiles(target, batchcmds, sourcefile_proto, opt, "cxx")
    end)
    on_buildcmd_file(function(target, batchcmds, sourcefile_proto, opt)
        import("proto").buildcmd_cxfiles(target, batchcmds, sourcefile_proto, opt, "cxx")
    end)
    on_build_file(function (target, sourcefile, opt)
    end)
    before_build_files(function (target, batchjobs, sourcebatch, opt)
        import("proto").build_cxfiles(target, batchjobs, sourcebatch, opt, "cxx")
    end, {batch = true})


-- define rule: protobuf.c
rule("protobuf.c")
    add_deps("c++")
    set_extensions(".proto")
    after_load(function(target)
        import("proto").load(target, "cc")
    end)
    before_buildcmd_file(function(target, batchcmds, sourcefile_proto, opt)
        import("proto").buildcmd_pfiles(target, batchcmds, sourcefile_proto, opt, "cc")
    end)
    on_buildcmd_file(function(target, batchcmds, sourcefile_proto, opt)
        import("proto").buildcmd_cxfiles(target, batchcmds, sourcefile_proto, opt, "cc")
    end)
    before_build_files(function (target, batchjobs, sourcebatch, opt)
        import("proto").build_cxfiles(target, batchjobs, sourcebatch, opt, "cc")
    end, {batch = true})
