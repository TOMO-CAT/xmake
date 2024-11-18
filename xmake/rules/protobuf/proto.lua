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
-- @file        proto.lua
--

-- imports
import("core.base.option")
import("lib.detect.find_tool")
import("core.project.depend")
import("private.action.build.object", { alias = "build_objectfiles" })
import("utils.progress")
import("private.utils.batchcmds")
import("private.async.buildjobs")

-- get protoc
function _get_protoc(target, sourcekind)
    -- find protoc
    -- @see https://github.com/xmake-io/xmake/issues/4659
    local envs = os.joinenvs(target:pkgenvs(), os.getenvs())
    local protoc = target:data("protobuf.protoc")
    if not protoc and sourcekind == "cxx" then
        protoc = find_tool("protoc", { envs = envs })
        if protoc and protoc.program then
            target:data_set("protobuf.protoc", protoc.program)
        end
    end

    -- find protoc-c
    local protoc_c = target:data("protobuf.protoc-c")
    if not protoc_c and sourcekind == "cc" then
        protoc_c = find_tool("protoc-c", { envs = envs }) or protoc
        if protoc_c and protoc_c.program then
            target:data_set("protobuf.protoc-c", protoc_c.program)
        end
    end

    -- get protoc
    return assert(target:data(sourcekind == "cxx" and "protobuf.protoc" or "protobuf.protoc-c"), "protoc not found!")
end

-- get grpc_cpp_plugin
function _get_grpc_cpp_plugin(target, sourcekind)
    local envs = os.joinenvs(target:pkgenvs(), os.getenvs())
    assert(sourcekind == "cxx", "grpc_cpp_plugin only support c++")
    local grpc_cpp_plugin = find_tool("grpc_cpp_plugin", { norun = true, force = true, envs = envs })
    return assert(grpc_cpp_plugin and grpc_cpp_plugin.program, "grpc_cpp_plugin not found!")
end

-- we need to add some configs to export includedirs to other targets in on_load
-- @see https://github.com/xmake-io/xmake/issues/2256
function load(target, sourcekind)
    -- get the first sourcefile
    local sourcefile_proto
    local sourcebatch = target:sourcebatches()[sourcekind == "cxx" and "protobuf.cpp" or "protobuf.c"]
    if sourcebatch then
        sourcefile_proto = sourcebatch.sourcefiles[1]
    end
    if not sourcefile_proto then
        return
    end

    -- get c/c++ source file for protobuf
    local prefixdir
    local autogendir
    local public
    local fileconfig = target:fileconfig(sourcefile_proto)
    if fileconfig then
        public = fileconfig.proto_public
        prefixdir = fileconfig.proto_rootdir
        autogendir = fileconfig.proto_autogendir
    end
    local rootdir = autogendir and autogendir or path.join(target:autogendir(), "rules", "protobuf")
    local filename = path.basename(sourcefile_proto) .. ".pb" .. (sourcekind == "cxx" and ".cc" or "-c.c")
    local sourcefile_cx = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename })
    local sourcefile_dir = prefixdir and path.join(rootdir, prefixdir) or path.directory(sourcefile_cx)

    -- add includedirs
    target:add("includedirs", sourcefile_dir, { public = public })
end

function buildcmd_pfiles(target, batchcmds, sourcefile_proto, opt, sourcekind, proto_dep_files)
    -- get protoc
    local protoc = _get_protoc(target, sourcekind)

    -- get c/c++ source file for protobuf
    local prefixdir
    local autogendir
    local public
    local grpc_cpp_plugin
    local extra_flags
    local fileconfig = target:fileconfig(sourcefile_proto)
    if fileconfig then
        public = fileconfig.proto_public
        prefixdir = fileconfig.proto_rootdir
        -- custom autogen directory to access the generated header files
        -- @see https://github.com/xmake-io/xmake/issues/3678
        autogendir = fileconfig.proto_autogendir
        grpc_cpp_plugin = fileconfig.proto_grpc_cpp_plugin
        -- extra flags for protoc
        -- @see https://github.com/TOMO-CAT/xmake/issues/23
        extra_flags = fileconfig.extra_flags
    end
    local rootdir = autogendir and autogendir or path.join(target:autogendir(), "rules", "protobuf")
    local filename = path.basename(sourcefile_proto) .. ".pb" .. (sourcekind == "cxx" and ".cc" or "-c.c")
    local sourcefile_cx = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename })
    local sourcefile_dir = prefixdir and path.join(rootdir, prefixdir) or path.directory(sourcefile_cx)

    local grpc_cpp_plugin_bin
    local filename_grpc
    local sourcefile_cx_grpc
    if grpc_cpp_plugin then
        grpc_cpp_plugin_bin = _get_grpc_cpp_plugin(target, sourcekind)
        filename_grpc = path.basename(sourcefile_proto) .. ".grpc.pb.cc"
        sourcefile_cx_grpc = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename_grpc })
    end

    local protoc_args = {
        path(sourcefile_proto),
        path(prefixdir and prefixdir or path.directory(sourcefile_proto), function(p) return "-I" .. p end),
        path(sourcefile_dir, function(p) return (sourcekind == "cxx" and "--cpp_out=" or "--c_out=") .. p end)
    }

    -- insert the custom extra-flags into protoc_args
    for _, v in pairs(table.wrap(extra_flags)) do
        table.insert(protoc_args, v)
    end

    if grpc_cpp_plugin then
        local extension = target:is_plat("windows") and ".exe" or ""
        table.insert(protoc_args, "--plugin=protoc-gen-grpc=" .. grpc_cpp_plugin_bin .. extension)
        table.insert(protoc_args, path(sourcefile_dir, function(p) return ("--grpc_out=") .. p end))
    end

    -- add commands
    batchcmds:mkdir(sourcefile_dir)
    batchcmds:show_progress(
        opt.progress,
        "${color.build.object}compiling.proto.%s %s",
        (sourcekind == "cxx" and "c++" or "c"),
        sourcefile_proto
    )
    batchcmds:vrunv(protoc, protoc_args, { colored_output = true })

    -- add deps
    --
    -- FIXME: if we use `extra_flags` parameter, some dependency information may be lost here,
    -- which could lead to issues with incremental compilation
    -- @see https://github.com/TOMO-CAT/xmake/issues/30
    local depmtime = os.mtime(sourcefile_cx)
    batchcmds:add_depfiles(sourcefile_proto)
    for _, dep_pb_file in ipairs(proto_dep_files) do
        batchcmds:add_depfiles(dep_pb_file)
    end
    batchcmds:set_depcache(target:dependfile(sourcefile_cx))
    if grpc_cpp_plugin then
        batchcmds:set_depmtime(math.max(os.mtime(sourcefile_cx_grpc), depmtime))
    else
        batchcmds:set_depmtime(depmtime)
    end
end

function buildcmd_cxfiles(target, batchcmds, sourcefile_proto, opt, sourcekind)
    -- get protoc
    local protoc = _get_protoc(target, sourcekind)

    -- get c/c++ source file for protobuf
    local prefixdir
    local autogendir
    local public
    local grpc_cpp_plugin
    local fileconfig = target:fileconfig(sourcefile_proto)
    if fileconfig then
        public = fileconfig.proto_public
        -- include *.pb.h according to it's relative path to fileconfig.proto_rootdir
        -- @see https://github.com/xmake-io/xmake/issues/828
        -- https://github.com/xmake-io/xmake/issues/1844
        -- https://github.com/TOMO-CAT/xmake/issues/22
        prefixdir = fileconfig.proto_rootdir
        -- custom autogen directory to access the generated header files
        -- @see https://github.com/xmake-io/xmake/issues/3678
        autogendir = fileconfig.proto_autogendir
        grpc_cpp_plugin = fileconfig.proto_grpc_cpp_plugin
    end
    local rootdir = autogendir and autogendir or path.join(target:autogendir(), "rules", "protobuf")
    local filename = path.basename(sourcefile_proto) .. ".pb" .. (sourcekind == "cxx" and ".cc" or "-c.c")
    local sourcefile_cx = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename })
    local sourcefile_dir = prefixdir and path.join(rootdir, prefixdir) or path.directory(sourcefile_cx)

    local grpc_cpp_plugin_bin
    local filename_grpc
    local sourcefile_cx_grpc
    if grpc_cpp_plugin then
        grpc_cpp_plugin_bin = _get_grpc_cpp_plugin(target, sourcekind)
        filename_grpc = path.basename(sourcefile_proto) .. ".grpc.pb.cc"
        sourcefile_cx_grpc = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename_grpc })
    end

    -- add includedirs
    target:add("includedirs", sourcefile_dir, { public = public })

    -- add objectfile
    local objectfile = target:objectfile(sourcefile_cx)
    table.insert(target:objectfiles(), objectfile)

    local objectfile_grpc
    if grpc_cpp_plugin then
        objectfile_grpc = target:objectfile(sourcefile_cx_grpc)
        table.insert(target:objectfiles(), objectfile_grpc)
    end

    batchcmds:show_progress(opt.progress, "${color.build.object}compiling.proto.$(mode) %s", sourcefile_cx)
    batchcmds:compile(sourcefile_cx, objectfile, { configs = { includedirs = sourcefile_dir } })
    if grpc_cpp_plugin then
        batchcmds:compile(sourcefile_cx_grpc, objectfile_grpc, { configs = { includedirs = sourcefile_dir } })
    end

    -- add deps
    --
    -- FIXME: if we use `extra_flags` parameter, some dependency information may be lost here,
    -- which could lead to issues with incremental compilation
    -- @see https://github.com/TOMO-CAT/xmake/issues/30
    local depmtime = os.mtime(objectfile)
    batchcmds:add_depfiles(sourcefile_proto)
    batchcmds:set_depcache(target:dependfile(objectfile))
    if grpc_cpp_plugin then
        batchcmds:set_depmtime(math.max(os.mtime(objectfile_grpc), depmtime))
    else
        batchcmds:set_depmtime(depmtime)
    end
end

function build_cxfile_objects(target, batchjobs, opt, sourcekind)
    -- do build
    local sourcebatch_cx = {
        rulename = (sourcekind == "cxx" and "c++" or "c") .. ".build",
        sourcekind = sourcekind,
        sourcefiles = {},
        objectfiles = {},
        dependfiles = {}
    }
    for _, sourcefile_proto in ipairs(sourcefiles) do
        -- get c/c++ source file for protobuf
        local prefixdir
        local autogendir
        local public
        local grpc_cpp_plugin
        local fileconfig = target:fileconfig(sourcefile_proto)
        if fileconfig then
            public = fileconfig.proto_public
            prefixdir = fileconfig.proto_rootdir
            -- custom autogen directory to access the generated header files
            -- @see https://github.com/xmake-io/xmake/issues/3678
            autogendir = fileconfig.proto_autogendir
            grpc_cpp_plugin = fileconfig.proto_grpc_cpp_plugin
        end
        local rootdir = autogendir and autogendir or path.join(target:autogendir(), "rules", "protobuf")
        local filename = path.basename(sourcefile_proto) .. ".pb" .. (sourcekind == "cxx" and ".cc" or "-c.c")
        local sourcefile_cx = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename })
        local sourcefile_dir = prefixdir and path.join(rootdir, prefixdir) or path.directory(sourcefile_cx)

        local grpc_cpp_plugin_bin
        local filename_grpc
        local sourcefile_cx_grpc
        if grpc_cpp_plugin then
            grpc_cpp_plugin_bin = _get_grpc_cpp_plugin(target, sourcekind)
            filename_grpc = path.basename(sourcefile_proto) .. ".grpc.pb.cc"
            sourcefile_cx_grpc = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename_grpc })
        end

        -- add includedirs
        target:add("includedirs", sourcefile_dir, { public = public })

        -- add objectfile
        local objectfile = target:objectfile(sourcefile_cx)
        local dependfile = target:dependfile(sourcefile_proto)
        table.insert(sourcebatch_cx.sourcefiles, sourcefile_cx)
        table.insert(sourcebatch_cx.objectfiles, objectfile)
        table.insert(sourcebatch_cx.dependfiles, dependfile)

        local objectfile_grpc
        if grpc_cpp_plugin then
            objectfile_grpc = target:objectfile(sourcefile_cx_grpc)
            table.insert(sourcebatch_cx.sourcefiles, sourcefile_cx_grpc)
            table.insert(sourcebatch_cx.objectfiles, objectfile_grpc)
            table.insert(sourcebatch_cx.dependfiles, dependfile)
        end
    end
    build_objectfiles(target, batchjobs, sourcebatch_cx, opt)
end

function build_cxfile(target, sourcefile_proto, opt, sourcekind)
    -- get c/c++ sourcefile
    local prefixdir
    local autogendir
    local public
    local grpc_cpp_plugin
    local fileconfig = target:fileconfig(sourcefile_proto)
    if fileconfig then
        public = fileconfig.proto_public
        prefixdir = fileconfig.proto_rootdir
        -- custom autogen directory to access the generated header files
        -- @see https://github.com/xmake-io/xmake/issues/3678
        autogendir = fileconfig.proto_autogendir
        grpc_cpp_plugin = fileconfig.proto_grpc_cpp_plugin
    end
    local rootdir = autogendir and autogendir or path.join(target:autogendir(), "rules", "protobuf")
    local filename = path.basename(sourcefile_proto) .. ".pb" .. (sourcekind == "cxx" and ".cc" or "-c.c")
    local sourcefile_cx = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename })
    local sourcefile_dir = prefixdir and path.join(rootdir, prefixdir) or path.directory(sourcefile_cx)
    if grpc_cpp_plugin then
        grpc_cpp_plugin_bin = _get_grpc_cpp_plugin(target, sourcekind)
        filename_grpc = path.basename(sourcefile_proto) .. ".grpc.pb.cc"
        sourcefile_cx_grpc = target:autogenfile(sourcefile_proto, { rootdir = rootdir, filename = filename_grpc })
    end

    -- add includedirs
    target:add("includedirs", sourcefile_dir, { public = public })

    -- add objectfile
    local objectfile = target:objectfile(sourcefile_cx)
    local dependfile = target:dependfile(sourcefile_proto)
    table.insert(target:objectfiles(), objectfile)

    local objectfile_grpc
    if grpc_cpp_plugin then
        objectfile_grpc = target:objectfile(sourcefile_cx_grpc)
        table.insert(target:objectfiles(), objectfile_grpc)
    end

    local build_opt = table.join({ objectfile = objectfile, dependfile = dependfile, sourcekind = sourcekind }, opt)
    import("private.action.build.object").build_object(target, sourcefile_cx, build_opt)
end

--  get all dependency .proto files of a given .proto file hierarchically
-- @see https://groups.google.com/g/protobuf/c/PKQde2JIDSo
function get_proto_dep_files(target, sourcefile_proto, proto_dep_files_cache, proto_includedirs, visited)
    -- protoc -I. -Ifoo --include_imports --descriptor_set_out=/dev/stdout proto/test.proto | protoc --decode=google.protobuf.FileDescriptorSet google/protobuf/descriptor.proto | grep "^  name: "

    local disable = false
    if disable then
        return
    end

    if visited[sourcefile_proto] then
        return
    end

    if not proto_dep_files_cache[sourcefile_proto] then
        proto_dep_files_cache[sourcefile_proto] = {}
    end

    local fileconfig = target:fileconfig(sourcefile_proto)
    local proto_rootdir = "."
    if fileconfig and fileconfig.proto_rootdir then
        proto_rootdir = fileconfig.proto_rootdir
    end

    local lines = io.lines(sourcefile_proto)
    for line in lines do
        local imported_file = line:match('^%s*import%s+"([^"]+)"%s*;')
        if imported_file then
            if proto_rootdir and os.exists(path.join(proto_rootdir, imported_file)) then
                -- try to find the imported file from proto_rootdir first
                local imported_file_path = path.join(proto_rootdir, imported_file)
                table.insert(proto_dep_files_cache[sourcefile_proto], imported_file_path)
                get_proto_dep_files(target, imported_file_path, proto_dep_files_cache, proto_includedirs, visited)
            else
                local find_improted_file = false
                -- try to find the imported file from proto_includedirs
                for _, proto_includedir in ipairs(proto_includedirs) do
                    local imported_file_path = path.join(proto_includedir, imported_file)
                    if os.exists(imported_file_path) then
                        find_improted_file = true
                        table.insert(proto_dep_files_cache[sourcefile_proto], imported_file_path)
                        get_proto_dep_files(target, imported_file_path, proto_dep_files_cache, proto_includedirs, visited)
                        break
                    end
                end
                if not find_improted_file then
                    cprint(
                        "${bright red}[rules@protobuf][error]${clear} cannot find imported file [" ..
                        imported_file .. "] for source proto [" .. sourcefile_proto .. "]")
                end
            end
        end
    end

    proto_dep_files_cache[sourcefile_proto] = table.unique(proto_dep_files_cache[sourcefile_proto])
    visited[sourcefile_proto] = true
end

-- build batch jobs
function build_cxfiles(target, batchjobs, sourcebatch, opt, sourcekind)
    import("core.project.config")

    opt = opt or {}
    local nodes = {}
    local nodenames = {}
    local node_rulename = "rules/" .. sourcebatch.rulename .. "/node"
    local sourcefiles = sourcebatch.sourcefiles

    -- parse fileconfig to get -I includedirs
    local proto_includedirs = {}
    for _, sourcefile_proto in ipairs(sourcefiles) do
        local fileconfig = target:fileconfig(sourcefile_proto)
        for _, v in pairs(table.wrap(fileconfig.extra_flags)) do
            if v:startswith("-I") then
                table.insert(proto_includedirs, v:sub(3))
            end
        end
    end
    proto_includedirs = table.unique(proto_includedirs)

    -- @key sourcefile_proto
    -- @value dep_proto_files
    local proto_dep_files_cache = {}
    local visited = {}
    for _, sourcefile_proto in ipairs(sourcefiles) do
        get_proto_dep_files(target, sourcefile_proto, proto_dep_files_cache, proto_includedirs, visited)
    end
    if config.get("debug") then
        io.save(path.join(config.debugdir(), "proto-dep-files", target:name() .. ".txt"), proto_dep_files_cache)
    end

    -- build proto && cx jobs
    for _, sourcefile_proto in ipairs(sourcefiles) do
        local nodename = node_rulename .. "/" .. sourcefile_proto
        nodes[nodename] = {
            name = nodename,
            job = batchjobs:addjob(nodename, function(index, total, jobopt)
                local batchcmds_ = batchcmds.new({ target = target })
                -- *.proto file ==> *.pb.h and *.pb.cc file
                buildcmd_pfiles(target, batchcmds_, sourcefile_proto, { progress = jobopt.progress }, sourcekind,
                    proto_dep_files_cache[sourcefile_proto])
                batchcmds_:runcmds({ changed = target:is_rebuilt(), dryrun = option.get("dry-run") })
            end)
        }
        table.insert(nodenames, nodename)

        local cxfile_nodename = nodename .. "/" .. sourcekind
        nodes[cxfile_nodename] = {
            name = cxfile_nodename,
            deps = { nodename },
            job = batchjobs:addjob(cxfile_nodename, function(index, total, jobopt)
                -- *.pb.cc file ==> object file
                build_cxfile(target, sourcefile_proto, { progress = jobopt.progress }, sourcekind)
            end)
        }
        table.insert(nodenames, cxfile_nodename)
    end
    buildjobs(nodes, batchjobs, opt.rootjob)
end
