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
-- @file        main.lua
--
-- imports
import("core.base.option")
import("core.base.global")
import("core.base.hashset")
import("core.project.config")
import("core.project.project")
import("core.platform.platform")
import("private.detect.find_platform")
import("core.cache.localcache")
import("scangen")
import("menuconf", {alias = "menuconf_show"})
import("configfiles", {alias = "generate_configfiles"})
import("private.action.require.register", {alias = "register_packages"})
import("private.action.require.install", {alias = "install_packages"})
import("private.service.remote_build.action", {alias = "remote_build_action"})
import("private.action.require.impl.utils.get_requires")
import("private.action.require.impl.package")

-- filter option
function _option_filter(name)
    local options = {
        target = true,
        file = true,
        root = true,
        yes = true,
        quiet = true,
        confirm = true,
        project = true,
        verbose = true,
        diagnosis = true,
        require = true,
        export = true,
        import = true,
        check = true,
        menu = true
    }
    return not options[name]
end

-- host changed?
function _host_changed()
    return os.host() ~= config.read("host")
end

-- need check
function _need_check(changed)
    if changed then
        return true
    end

    -- clean?
    if option.get("clean") or option.get("check") then
        return true
    end

    -- check for all project files
    local mtimes = project.mtimes()
    local mtimes_prev = localcache.get("config", "mtimes")
    if mtimes_prev then
        for file, mtime in pairs(mtimes) do
            -- modified? reconfig and rebuild it
            local mtime_prev = mtimes_prev[file]
            if not mtime_prev or mtime > mtime_prev then
                return true
            end
        end
    end

    -- unfinished config/recheck
    if localcache.get("config", "recheck") then
        return true
    end

    -- has xmake been updated? force to check config again
    -- we need to clean the dirty config cache of the old version
    if os.mtime(path.join(os.programdir(), "core", "main.lua")) >
        os.mtime(config.filepath()) then
        return true
    end

    -- xmake package has been deleted / broken?
    local package_cache = localcache.cache("package")
    local package_dirs = localcache.cache("references")
    package_cache = package_cache and package_cache:data()
    package_dirs = package_dirs and package_dirs:data()
    package_dirs = package_dirs and package_dirs.packages

    if package_cache and package_dirs then
        -- detect broken pkg dir
        local package_broken_path = {}
        local function _check_package_broken_dir(pkg_name, dirs)
            for _, dir in ipairs(table.wrap(dirs)) do
                if dir and not os.isdir(dir) then
                    table.insert(package_broken_path, dir)
                end
            end
        end
        for pkg_name, pkg in pairs(package_cache) do
            _check_package_broken_dir(pkg_name, pkg.installdir)
            _check_package_broken_dir(pkg_name, pkg.includedirs)
            _check_package_broken_dir(pkg_name, pkg.sysincludedirs)
            _check_package_broken_dir(pkg_name, pkg.linkdirs)
        end

        -- if package:installdir is broken, we need to delete installdir to trigger a forced reinstallation.
        local need_delete_package_dirs = {}
        for _, pkg_dir in ipairs(package_dirs) do
            for _, broken_path in ipairs(package_broken_path) do
                if broken_path:startswith(pkg_dir) then
                    -- cprint("${color.warning} pkg dir [%s] is broken", broken_path)
                    table.insert(need_delete_package_dirs, pkg_dir)
                    break
                end
            end
        end

        -- remove broken packages
        for _, pkg_dir in ipairs(need_delete_package_dirs) do
            cprint(
                "${color.warning} pkg [%s] have been broken, we will remove it's installdir and reinstall it later",
                pkg_dir)
            os.tryrm(pkg_dir)
        end

        if #need_delete_package_dirs > 0 then
            return true
        end
    end

    local requires, requires_extra = get_requires(requires_raw)
    if requires and #requires > 0 then
        local packages = package.load_packages(requires, {
            requires_extra = requires_extra
        })
        for _, pkg in ipairs(packages) do
            -- have package with "package.install_always" policy?
            -- @see https://github.com/TOMO-CAT/xmake/issues/108
            if pkg and pkg:policy("package.install_always") then
                -- if package with "package.install_always" policy have been installed, then we don't need to recheck it
                -- @see https://github.com/TOMO-CAT/xmake/issues/107
                local install_package_result_mtime = os.getenv(
                                                         "XMAKE_INSTALL_PACKAGES_RESULT")
                if not install_package_result_mtime then
                    return true
                else
                    local master_misccache = import(
                                                 "core.cache.master_misccache",
                                                 {anonymous = true})
                    local key = "install_packages_result"
                    local cacheinfo = master_misccache:get2(key,
                                                            install_package_result_mtime) or
                                          {}

                    local pkg_name = pkg:name()
                    local pkg_installdir = pkg:installdir()
                    if not cacheinfo[pkg_name] then
                        return true
                    elseif not cacheinfo[pkg_name][pkg_installdir] then
                        return true
                    end
                end
            end
        end
    end

    return false
end

-- check target
function _check_target(target, checked_targets)
    if not checked_targets[target:name()] then
        checked_targets[target:name()] = target
        for _, depname in ipairs(target:get("deps")) do
            assert(depname ~= target:name(),
                   "the target(%s) cannot depend self!", depname)
            local deptarget = project.target(depname)
            assert(deptarget, "unknown target(%s) for %s.deps!", depname,
                   target:name())
            _check_target(deptarget, checked_targets)
        end
    end
end

-- check targets
function _check_targets()
    assert(not project.is_loaded(),
           "project and targets may have been loaded early!")
    local checked_targets = {}
    for _, target in pairs(project.targets()) do
        _check_target(target, checked_targets)
    end
end

-- check target toolchains
function _check_target_toolchains()
    -- check toolchains configuration for all target in the current project
    -- @note we must check targets after loading options
    for _, target in pairs(project.targets()) do
        if target:is_enabled() and
            (target:get("toolchains") or not target:is_plat(config.get("plat")) or
                not target:is_arch(config.get("arch"))) then

            -- check platform toolchains first
            -- `target/set_plat()` and target:toolchains() need it
            target:platform():check()

            -- check target toolchains next
            local target_toolchains = target:get("toolchains")
            if target_toolchains then
                target_toolchains = hashset.from(table.wrap(target_toolchains))
                for _, toolchain_inst in pairs(target:toolchains()) do
                    -- check toolchains for `target/set_toolchains()`
                    if not toolchain_inst:check() and
                        target_toolchains:has(toolchain_inst:name()) then
                        raise("toolchain(\"%s\"): not found!",
                              toolchain_inst:name())
                    end
                end
            end
        elseif not target:get("toolset") then
            -- we only abort it when we know that toolchains of platform and target do not found
            local toolchain_found
            for _, toolchain_inst in pairs(target:toolchains()) do
                if toolchain_inst:is_standalone() then
                    toolchain_found = true
                end
            end
            assert(toolchain_found, "target(%s): toolchain not found!",
                   target:name())
        end
    end
end

-- find default mode
function _find_default_mode()
    local mode = config.mode()
    if not mode then
        mode = project.get("defaultmode")
        if not mode then
            mode = "release"
        end
        config.set("mode", mode)
    end
    return mode
end

-- check configs
function _check_configs()
    -- check allowed modes
    local mode = config.mode()
    local allowed_modes = project.allowed_modes()
    if allowed_modes then
        if not allowed_modes:has(mode) then
            local allowed_modes_str = table.concat(allowed_modes:to_array(),
                                                   ", ")
            raise(
                "`%s` is not a valid complation mode for this project, please use one of %s",
                mode, allowed_modes_str)
        end
    end

    -- check allowed plats
    local plat = config.plat()
    local allowed_plats = project.allowed_plats()
    if allowed_plats then
        if not allowed_plats:has(plat) then
            local allowed_plats_str = table.concat(allowed_plats:to_array(),
                                                   ", ")
            raise(
                "`%s` is not a valid platform for this project, please use one of %s",
                plat, allowed_plats_str)
        end
    end

    -- check allowed archs
    local arch = config.arch()
    local allowed_archs = project.allowed_archs(config.plat())
    if allowed_archs then
        if not allowed_archs:has(arch) then
            local allowed_archs_str = table.concat(allowed_archs:to_array(),
                                                   ", ")
            raise(
                "`%s` is not a valid complation arch for this project, please use one of %s",
                arch, allowed_archs_str)
        end
    end
end

-- export configs
function _export_configs()
    local exportfile = option.get("export")
    if exportfile then
        config.save(exportfile, {public = true})
    end
end

function main(opt)

    -- do action for remote?
    if remote_build_action.enabled() then
        return remote_build_action()
    end

    -- avoid running this task repeatedly
    opt = opt or {}
    if _g.configured then
        return
    end
    _g.configured = true

    -- scan project and generate it if xmake.lua not exists
    local autogen = false
    local trybuild = option.get("trybuild")
    if not os.isfile(project.rootfile()) and not trybuild then
        autogen = utils.confirm({
            default = false,
            description = "xmake.lua not found, try generating it"
        })
        if autogen then
            scangen()
        else
            os.exit()
        end
    end

    -- check the working directory
    if not option.get("project") and not option.get("file") and -- no given project path
        not localcache.get("project", "projectdir") and -- no cached project path
        not localcache.get("project", "projectfile") and
        os.isdir(os.projectdir()) then
        if path.translate(os.projectdir()) ~= path.translate(os.workingdir()) then
            wprint(
                [[You are working in the project directory(%s) and you can also
force to build in current directory via run `xmake -P .`]], os.projectdir())
        end
    end

    -- lock the whole project
    project.lock()

    -- enter menu config
    local options_changed = false
    if option.get("menu") then
        options_changed = menuconf_show()
    end

    -- load the project configuration
    --
    -- priority: option > option_cache > global > option_default > config_check > project_check > config_cache
    --

    -- get the options
    local options = nil
    for name, value in pairs(option.options()) do
        if _option_filter(name) then
            options = options or {}
            options[name] = value
        end
    end

    -- merge options from the given import file
    local importfile = option.get("import")
    if importfile then
        assert(os.isfile(importfile), "%s not found!", importfile)
        -- we need to use readonly, @see https://github.com/xmake-io/xmake/issues/2278
        local import_configs = io.load(importfile)
        if import_configs then
            for name, value in pairs(import_configs) do
                options = options or {}
                if options[name] == nil then
                    options[name] = value
                end
            end
        end
    end

    -- override configuration from the options or cache
    --
    -- retrieve the history option from `.xmake/${plat}/${arch}/cache/config` and then
    -- determine if the option has been changed
    local options_history = {}
    if not option.get("clean") and not autogen then
        options_history = localcache.get("config", "options") or {}
        options = options or options_history
    end
    for name, value in pairs(options) do
        -- Is options changed by argument options?
        options_changed = options_changed or options_history[name] ~= value
        -- @note override it and mark as readonly (highest priority)
        config.set(name, value, {readonly = true})
    end

    -- merge the cached configuration
    --
    -- @note we cannot load cache config when switching platform, arch ..
    -- so we need known whether options have been changed
    --
    local configcache_loaded = false
    if not options_changed and not option.get("clean") and not _host_changed() then
        configcache_loaded = config.load()
    end

    -- merge the global configuration
    for name, value in pairs(global.options()) do
        if config.get(name) == nil then
            config.set(name, value)
        end
    end

    -- merge the default options
    for name, value in pairs(option.defaults()) do
        if _option_filter(name) and config.get(name) == nil then
            config.set(name, value)
        end
    end

    -- merge the project options after default options
    for name, value in pairs(project.get("config")) do
        value = table.unwrap(value)
        assert(type(value) == "string" or type(value) == "boolean" or
                   type(value) == "number",
               "set_config(%s): unsupported value type(%s)", name, type(value))
        if not config.readonly(name) then
            config.set(name, value)
        end
    end

    -- find default mode
    local mode = _find_default_mode()
    assert(mode == config.mode())

    -- find default platform and save to configuration
    local plat, arch = find_platform({global = true})
    assert(plat == config.plat())
    assert(arch == config.arch())

    -- load platform instance
    local instance_plat = platform.load(plat, arch)

    -- merge the checked configuration
    local recheck = _need_check(options_changed or not configcache_loaded or
                                    autogen)
    if recheck then

        -- clear cached configuration
        if option.get("clean") then
            localcache.clear("config")
        end

        -- clear some local caches
        localcache.clear("detect")
        localcache.clear("option")
        localcache.clear("package")
        localcache.clear("toolchain")
        localcache.clear("cxxmodules")
        localcache.set("config", "recheck", true)
        localcache.save()

        -- check platform
        instance_plat:check()

        -- check project options
        if not trybuild then
            project.check_options()
        end
    end

    -- translate the build directory
    local buildir = config.get("buildir")
    if buildir and path.is_absolute(buildir) then
        config.set("buildir", path.relative(buildir, project.directory()),
                   {readonly = true, force = true})
    end

    -- only config for building project using third-party buildsystem
    if not trybuild then

        -- check configs
        _check_configs()

        -- install and register packages
        local require_enable = option.boolean(option.get("require"))
        if (recheck or require_enable) then
            if require_enable ~= false then
                install_packages()
            else
                register_packages()
            end
        end

        -- check target and ensure to load all targets, @note we must load targets after installing required packages,
        -- otherwise has_package() will be invalid.
        _check_targets()

        -- check target toolchains
        if recheck then
            _check_target_toolchains()
        end

        -- load targets
        project.load_targets({recheck = recheck})

        if project.policy("package.enable_softlink_installdir") then
            -- after install xrepo packages, we will call `os.tryrm()` to delete `package:data("cleanable_sourcedir")`,
            -- and deleting the sourcedir will remove the installdir of the dependent packages that
            -- are symlibked in `${sourcedir}/build/.pkg`, so we choose not to create softlink in XREPO
            if not os.getenv("XMAKE_IN_XREPO") then
                -- mkdir a softlink to all packages' installdir to avoid using absolute paths with the __FILE__ macro
                -- @see https://github.com/TOMO-CAT/xmake/issues/62
                local requires = project.required_packages()
                if requires then
                    for requirename, require in pairs(requires) do
                        local pkg_softlink_installdir = path.join(
                                                            config.buildir(),
                                                            ".pkg", requirename)
                        local pkg_installdir = path.absolute(
                                                   require:installdir())
                        -- when we `add_requires("zlib")` in a project, require:installdir() may be nil
                        -- this may be because this library is a system library
                        if pkg_installdir then
                            -- remove previous softlink to force update the symbolic link of package installdir
                            os.rm(pkg_softlink_installdir)
                            os.ln(pkg_installdir, pkg_softlink_installdir)
                        end
                    end
                end
            end
        end

        -- update the config files
        generate_configfiles({force = recheck})
    end

    -- dump config
    if option.get("verbose") and not opt.disable_dump then
        config.dump()
    end

    -- export configs
    if option.get("export") then
        _export_configs()
    end

    -- we need to save it and enable external working mode
    -- if we configure the given project directory
    --
    -- @see https://github.com/xmake-io/xmake/issues/3342
    --
    local projectdir = option.get("project")
    local projectfile = option.get("file")
    if projectdir or projectfile then
        localcache.set("project", "projectdir", projectdir)
        localcache.set("project", "projectfile", projectfile)
        localcache.save("project")
    end

    -- save options and config cache
    localcache.set("config", "recheck", false)
    localcache.set("config", "mtimes", project.mtimes())
    config.save()
    localcache.set("config", "options", options)
    localcache.save("config")

    -- unlock the whole project
    project.unlock()
end
