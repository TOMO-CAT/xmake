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
-- @file        find_package.lua
--

-- imports
import("core.base.option")
import("core.project.target")
import("lib.detect.find_tool")
import("private.core.base.is_cross")
import("package.manager.pkgconfig.find_package", {alias = "find_package_from_pkgconfig"})

-- get result from list of file inside pacman package
function _find_package_from_list(list, name, pacman, opt)

    local pathtomsys = nil

    -- iterate over each file path inside the pacman package
    local result = {includedirs = {}, linkdirs = {}, links = {}}
    for _, line in ipairs(list:split('\n', {plain = true})) do
        line = line:trim():split('%s+')[2]
        if line:find("/include/", 1, true) and (line:endswith(".h") or line:endswith(".hpp")) then
            if not line:startswith("/usr/include/") then
                local hpath = line
                table.insert(result.includedirs, path.directory(hpath))
            end
        -- remove lib and .a, .dll.a and .so to have the links
        elseif line:endswith(".so") then
            table.insert(result.linkdirs, path.directory(line))
            table.insert(result.links, target.linkname(path.filename(line), {plat = opt.plat}))
        elseif line:endswith(".a") then
            local apath = line
            table.insert(result.linkdirs, path.directory(apath))
            table.insert(result.links, target.linkname(path.filename(apath), {plat = opt.plat}))
        end
    end
    result.includedirs = table.unique(result.includedirs)
    result.linkdirs = table.unique(result.linkdirs)
    result.links = table.reverse_unique(result.links)

    -- use pacman package version as version
    local version = try { function() return os.iorunv(pacman.program, {"-Q", name}) end }
    if version then
        version = version:trim():split('%s+')[2]
        -- we need strip "1:" prefix, @see https://github.com/xmake-io/xmake/issues/2020
        -- e.g. vulkan-headers 1:1.3.204-1
        if version:startswith("1:") then
            version = version:split(':')[2]
        end
        result.version = version:split('-')[1]
    else
        result = nil
    end
    return result
end

-- find package from the system directories
--
-- @param name  the package name
-- @param opt   the options, e.g. {verbose = true, version = "1.12.x")
--
function main(name, opt)
    opt = opt or {}
    if is_cross(opt.plat, opt.arch) then
        return
    end

    -- find pacman
    local pacman = find_tool("pacman")
    if not pacman then
        return
    end

    -- get package files list
    list = name and try { function() return os.iorunv(pacman.program, {"-Q", "-l", name}) end }
    if not list then
        return
    end

    -- parse package files list
    local linkdirs = {}
    local pkgconfig_files = {}
    for _, line in ipairs(list:split('\n', {plain = true})) do
        line = line:trim():split('%s+')[2]
        if line:find("/pkgconfig/", 1, true) and line:endswith(".pc") then
            table.insert(pkgconfig_files, line)
        end
        if line:endswith(".so") or line:endswith(".a") or line:endswith(".lib") then
            table.insert(linkdirs, path.directory(line))
        end
    end
    linkdirs = table.unique(linkdirs)

    -- we iterate over each pkgconfig file to extract the required data
    local foundpc = false
    local result = {includedirs = {}, linkdirs = {}, links = {}}
    for _, pkgconfig_file in ipairs(pkgconfig_files) do
        local pkgconfig_dir = path.directory(pkgconfig_file)
        local pkgconfig_name = path.basename(pkgconfig_file)
        local pcresult = find_package_from_pkgconfig(pkgconfig_name, {configdirs = pkgconfig_dir, linkdirs = linkdirs})

        -- the pkgconfig file has been parse successfully
        if pcresult then
            for _, includedir in ipairs(pcresult.includedirs) do
                table.insert(result.includedirs, includedir)
            end
            for _, linkdir in ipairs(pcresult.linkdirs) do
                table.insert(result.linkdirs, linkdir)
            end
            for _, link in ipairs(pcresult.links) do
                table.insert(result.links, link)
            end
            -- version should be the same if a pacman package contains multiples .pc
            result.version = pcresult.version
            foundpc = true
        end
    end

    if foundpc == true then
        result.includedirs = table.unique(result.includedirs)
        result.linkdirs = table.unique(result.linkdirs)
        result.links = table.reverse_unique(result.links)
    else
        -- if there is no .pc, we parse the package content to obtain the data we want
        result = _find_package_from_list(list, name, pacman, opt)
    end
    return result
end
