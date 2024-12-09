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
-- @file        install.lua
--

-- imports
import("core.base.option")
import("core.base.task")
import("lib.detect.find_tool")
import("core.project.config")
import("private.action.require.impl.package")
import("private.action.require.impl.repository")
import("private.action.require.impl.environment")
import("private.action.require.impl.install_packages")
import("private.action.require.impl.utils.get_requires")

-- check missing packages
function _check_missing_packages(packages)

    -- get all missing packages
    local packages_missing = {}
    local optional_missing = {}
    for _, instance in ipairs(packages) do
        if package.should_install(instance, {install_finished = true}) or (instance:is_fetchonly() and not instance:exists()) then
            if instance:is_optional() then
                optional_missing[instance:name()] = instance
            else
                table.insert(packages_missing, instance:name())
            end
        end
    end

    -- raise tips
    if #packages_missing > 0 then
        local cmd = "xmake repo -u"
        if os.getenv("XREPO_WORKING") then
            cmd = "xrepo update-repo"
        end
        raise("The packages(%s) not found, please run `%s` first!", table.concat(packages_missing, ", "), cmd)
    end

    -- save the optional missing packages
    _g.optional_missing = optional_missing
end

-- install packages
function main(requires_raw)

    -- get requires and extra config
    local requires_extra = nil
    local requires, requires_extra = get_requires(requires_raw)
    if not requires or #requires == 0 then
        return
    end

    -- find git
    environment.enter()
    local git = find_tool("git")
    environment.leave()

    -- pull all repositories first if not exists
    --
    -- attempt to install git from the builtin-packages first if git not found
    --
    if git and (not repository.pulled() or option.get("upgrade")) then
        task.run("repo", {update = true})
    end

    -- since install-package may operate across processes,
    -- we need to write the installed packages info into local-cache.
    -- @see https://github.com/TOMO-CAT/xmake/issues/107
    --
    -- We choose to set the install-packages information before install_packages to ensure that the xmake parent 
    -- process sets the XMAKE_INSTALL_PACKAGES_RESULT environment variable, thereby ensuring that this information 
    -- is shared throughout the entire xmake build process.
    --
    -- skip `xrepo env -b emmylua_debugger` situation
    if not os.getenv("XREPO_OUTSIDE_PROGRAM") then
        if not os.getenv("XMAKE_INSTALL_PACKAGES_RESULT") then
            local key = "install_packages_result"
            local mtime = tostring(math.floor(os.mclock()))
            os.setenv("XMAKE_INSTALL_PACKAGES_RESULT", mtime)
            os.setenv("XMAKE_MASTER_PROJECT_CACHE_DIR", path.absolute(config.cachedir()))

            local master_misccache = import("core.cache.master_misccache", {anonymous = true})

            -- clear the history cache
            local cacheinfo = {}
            cacheinfo[mtime] = {}
            master_misccache:set(key, cacheinfo)
            master_misccache:save()
        end
    end

    -- install packages
    environment.enter()
    local packages = install_packages(requires, {requires_extra = requires_extra})
    if packages then
        _check_missing_packages(packages)
    end
    environment.leave()

    -- dump install-package-result to misc local-cache
    -- @see https://github.com/TOMO-CAT/xmake/issues/107
    local install_package_result_mtime = os.getenv("XMAKE_INSTALL_PACKAGES_RESULT")
    if install_package_result_mtime then
        local master_misccache = import("core.cache.master_misccache", {anonymous = true})

        local key = "install_packages_result"
        local cacheinfo = master_misccache:get2(key, install_package_result_mtime) or {}
        for _, instance in ipairs(packages) do
            -- to avoid affecting too many packages, we only add packages with install-always policy
            if instance:policy("package.install_always") and instance:installdir() then
                local pkg_name = instance:name()
                local pkg_installdir = instance:installdir()
                cacheinfo[pkg_name] = cacheinfo[pkg_name] or {}
                cacheinfo[pkg_name][pkg_installdir] = {}
            end
        end
        master_misccache:set2(key, install_package_result_mtime, cacheinfo)
        master_misccache:save()
    end
end
