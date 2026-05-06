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
-- @file        main.lua
--

-- imports
import("core.base.semver")
import("core.base.option")
import("core.base.task")
import("core.base.process")
import("core.base.tty")
import("net.http")
import("devel.git")
import("net.fasturl")
import("core.base.privilege")
import("privilege.sudo")
import("async.runjobs")
import("private.action.require.impl.environment")
import("private.action.update.fetch_version")
import("utils.archive")
import("lib.detect.find_file")
import("lib.detect.find_tool")

-- run program with privilege
function _sudo_v(program, params)

    -- already root?
    if os.isroot() then
        os.vrunv(program, params)
        return true
    end

    -- try get privilege
    if privilege.get() then
        local ok = try
        {
            function ()
                os.vrunv(program, params)
                return true
            end
        }
        privilege.store()
        if ok then
            return true
        end
    end

    -- show tips
    local command = program .. " " .. os.args(params)

    -- continue to install with administrator permission?
    if sudo.has() then
        local confirm = utils.confirm({ default = true, description = format("try continue to run `%s` with administrator permission again", command) })
        if confirm then
            sudo.vrunv(program, params)
            return true
        end
    end
    return false
end

-- do install
function _install(sourcedir)

    -- the install task
    local install_task = function ()

        -- get the install directory
        local installdir = "/usr/local/bin"

        local ok = try
        {
            function ()
                os.cd(sourcedir)
                if not os.isfile("./scripts/install.sh") then
                    raise("scripts/install.sh not found in %s", sourcedir)
                end

                local runtime = option.get("runtime") or "luajit"
                os.vrunv("bash", {"./scripts/install.sh", "--runtime=" .. runtime})
                return true
            end,
            catch
            {
                function (errors)
                    vprint(errors)
                end
            }
        }

        if ok then
            cprint("${yellow}  => ${clear}install to %s .. ${color.success}${text.success}", installdir)
        else
            cprint("${yellow}  => ${clear}install to %s .. ${color.failure}${text.failure}", installdir)
            raise("install failed!")
        end
    end

    -- do install
    if option.get("verbose") then
        install_task()
    else
        runjobs("update/install", install_task, {progress = true})
    end
end

-- do install script
function _install_script(sourcedir)
    local source = path.join(sourcedir, "xmake")
    local program_dir = "/usr/local/share/xmake"

    local ok = try
    {
        function ()
            local script = path.join(program_dir, "scripts", "update-script.sh")
            cprint("${dim}[update]${clear} sh %s %s %s", script, program_dir, source)
            if not option.get("dry-run") then
                return _sudo_v("sh", { script, program_dir, source })
            end
            return true
        end,
        catch
        {
            function (errors)
                vprint(errors)
            end
        }
    }
    
    if ok then
        cprint("${yellow}  => ${clear}install script to %s .. ${color.success}${text.success}", program_dir)
    else
        cprint("${yellow}  => ${clear}install script to %s .. ${color.failure}${text.failure}", program_dir)
        raise("install script failed!")
    end
end

function _check_repo(sourcedir)
    -- this file will exists for long time
    if not os.isfile(path.join(sourcedir, "xmake/core/_xmake_main.lua")) then
        raise("invalid xmake repo, please check your input!")
    end
end

-- main
function main()
    -- enter environment
    environment.enter()

    -- has been installed?
    local fetchinfo   = assert(fetch_version(option.get("xmakever")), "cannot fetch xmake version info!")
    local mainurl     = fetchinfo.url
    local version     = fetchinfo.version
    local is_commit   = fetchinfo.is_commit
    if not is_commit and xmake.version():eq(version) and not option.get("force") then
        cprint("${bright}xmake %s has been installed!", version)
        return
    end

    local script_only = option.get("scriptonly")

    -- trace
    cprint("${dim}[update]${clear} url: ${green}%s${clear}, version: ${green}%s${clear}, force: ${green}%s${clear}, scriptonly: ${green}%s${clear}",
        mainurl, version, tostring(option.get("force") or false), tostring(option.get("scriptonly") or false))

    -- get the temporary source directory without ramdisk (maybe too large)
    local sourcedir = path.join(os.tmpdir({ramdisk = false}), "xmakesrc", version)
    vprint("prepared to download to temp dir %s ..", sourcedir)

    -- all user provided urls are considered as git url since check has been performed in fetch_version
    local install_from_git = git.checkurl(mainurl)

    -- the download task
    local download_task = function ()
        tty.erase_line_to_start().cr()
        cprintf("${yellow}  => ${clear}downloading %s .. ", mainurl)
        local ok = try
        {
            function ()
                if option.get("dry-run") then
                    return true
                end
                os.tryrm(sourcedir)
                if is_commit then
                    git.clone(mainurl, {recurse_submodules = not script_only, outputdir = sourcedir})
                    git.checkout(version, {repodir = sourcedir})
                else
                    git.clone(mainurl, {depth = 1, recurse_submodules = not script_only, branch = version, outputdir = sourcedir})
                end
                return true
            end,
            catch
            {
                function (errors)
                    vprint(errors)
                end
            }
        }
        tty.erase_line_to_start().cr()
        if ok then
            cprint("${yellow}  => ${clear}download %s .. ${color.success}${text.success}", mainurl)
        else
            cprint("${yellow}  => ${clear}download %s .. ${color.failure}${text.failure}", mainurl)
            raise("download failed!")
        end
    end

    -- do download
    if option.get("verbose") then
        download_task()
    else
        runjobs("update/download", download_task, {progress = true})
    end

    -- leave environment
    environment.leave()

    if install_from_git and not option.get("dry-run") then
        _check_repo(sourcedir)
    end

    -- do install
    if script_only then
        _install_script(sourcedir)
    else
        _install(sourcedir)
    end
end
