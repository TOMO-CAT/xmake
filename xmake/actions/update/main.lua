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

    -- attempt to install directly
    return try
    {
        function ()
            os.vrunv(program, params)
            return true
        end,

        catch
        {
            -- failed or not permission? request administrator permission and run it again
            function (errors)

                -- trace
                vprint(errors)

                -- try get privilege
                if privilege.get() then
                    local ok = try
                    {
                        function ()
                            os.vrunv(program, params)
                            return true
                        end
                    }

                    -- release privilege
                    privilege.store()

                    -- ok?
                    if ok then
                        return true
                    end
                end

                -- show tips
                local command = program .. " " ..os.args(params)
                cprint("\r${bright color.error}error: ${clear}run `%s` failed, may permission denied!", command)

                -- continue to install with administrator permission?
                if sudo.has() then

                    -- confirm to install?
                    local confirm = utils.confirm({ default = true, description = format("try continue to run `%s` with administrator permission again", command) })
                    if confirm then
                        sudo.vrunv(program, params)
                        return true
                    end
                end
            end
        }
    }
end

-- do uninstall
function _uninstall()

    -- remove shell profile
    local profiles = {}
    for _, filename in ipairs({".zshrc", ".bashrc", ".kshrc", ".bash_profile", ".profile"}) do
        table.insert(profiles, path.join(os.getenv("HOME"), filename))
    end
    for _, profile in ipairs(profiles) do
        if os.isfile(profile) then
            io.gsub(profile, "# >>> xmake >>>.-# <<< xmake <<<", "")
        end
    end

    -- remove program
    if os.programdir():startswith("/usr/") then
        _sudo_v(os.programfile(), {"lua", "rm", os.programdir()})
        for _, f in ipairs({"/usr/local/bin/xmake", "/usr/local/bin/xrepo", "/usr/bin/xmake", "/usr/bin/xrepo"}) do
            if os.isfile(f) then
                _sudo_v(os.programfile(), {"lua", "rm", f})
            end
        end
    else
        os.rm(os.programdir())
        os.rm(os.programfile())
        os.rm("~/.local/bin/xmake")
        os.rm("~/.local/bin/xrepo")
    end
end

-- do install
function _install(sourcedir)

    -- the install task
    local install_task = function ()

        -- get the install directory
        local installdir = "/usr/local/bin"

        -- trace
        tty.erase_line_to_start().cr()
        cprintf("${yellow}  => ${clear}installing to %s .. ", installdir)
        local ok = try
        {
            function ()

                -- install it
                os.cd(sourcedir)
                if os.isfile("./configure") then
                    os.vrun("./configure", {"--mode=release", "--runtime=lua"})
                end
                os.vrunv("make", {"-j4"})
                -- an error will be reported: cannot create regular file '/usr/local/bin/xmake': Text file busy
                -- _sudo_v("make", {"install", "VERBOSE=1", "PREFIX=/usr/local"})
                process.openv("make", {"install", "VERBOSE=1", "PREFIX=/usr/local"}, {stdout = os.tmpfile(), stderr = os.tmpfile(), detach = true}):close()
                return true
            end,
            catch
            {
                function (errors)
                    vprint(errors)
                end
            }
        }

        -- trace
        if ok then
            tty.erase_line_to_start().cr()
            cprint("${yellow}  => ${clear}install to %s .. ${color.success}${text.success}", installdir)
        else
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

    -- trace
    cprintf("\r${yellow}  => ${clear}install script to %s .. ", os.programdir())

    local source = path.join(sourcedir, "xmake")
    local dirname = path.filename(os.programdir())
    if dirname ~= "xmake" then
        local target = path.join(sourcedir, dirname)
        os.mv(source, target)
        source = target
    end

    local ok = try
    {
        function ()
            local script = path.join(os.programdir(), "scripts", "update-script.sh")
            return _sudo_v("sh", { script, os.programdir(), source })
        end,
        catch
        {
            function (errors)
                vprint(errors)
            end
        }
    }
    -- trace
    if ok then
        cprint("${color.success}${text.success}")
    else
        cprint("${color.failure}${text.failure}")
    end
end

-- initialize shells
function _initialize_shell()

    local target, command
    
    target = "~/.profile"
    local shell = os.shell()
    if shell:endswith("bash") then target = (is_host("macosx") and "~/.bash_profile" or "~/.bashrc")
    elseif shell:endswith("zsh") then target = "~/.zshrc"
    elseif shell:endswith("ksh") then target = "~/.kshrc"
    elseif shell:endswith("fish") then target = "~/.config/fish/config.fish"
    end
    local profile_home = path.absolute("~/.xmake/profile")
    command = ("test -f \"%s\" && source \"%s\""):format(profile_home, profile_home)

    -- write home profile
    local profile = "$XMAKE_PROGRAM_DIR/scripts/profile-unix.sh"
    local profile_fish = "$XMAKE_PROGRAM_DIR/scripts/profile-unix.fish"
    local bridge_command = format([[export XMAKE_ROOTDIR="%s"
export XMAKE_PROGRAM_DIR="%s"
# export PATH="$XMAKE_ROOTDIR:$PATH"
test $FISH_VERSION && test -f "%s" && source "%s" && exit 0
test -f "%s" && source "%s"
]], path.directory(os.programfile()), os.programdir(), profile_fish, profile_fish, profile, profile)
    io.writefile(profile_home, bridge_command)

    -- trace
    cprintf("\r${yellow}  => ${clear}installing shell integration to %s .. ", target)

    local ok = try
    {
        function ()
            local file = ""
            if os.isfile(target) then
                file = io.readfile(target)
                file = file:gsub("# >>> xmake >>>.-# <<< xmake <<<", "")
                if file ~= "" and not file:endswith("\n") then
                    file = file .. "\n"
                end
            end
            file = file .. "# >>> xmake >>>\n" .. command .. "\n# <<< xmake <<<"
            io.writefile(target, file)
            return true
        end,
        catch
        {
            function (errors)
                vprint(errors)
            end
        }
    }
    -- trace
    if ok then
        cprint("${color.success}${text.success}")
        print("Reload shell profile by running the following command now!")
        cprint("${bright}source ~/.xmake/profile${clear}")
    else
        cprint("${color.failure}${text.failure}")
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

    -- only uninstall it
    if option.get("uninstall") then

        -- do uninstall
        _uninstall()

        -- trace
        cprint("${color.success}uninstall ok!")
        return
    end

    -- initialize for shell interaction
    if option.get("integrate") then
        _initialize_shell()
        return
    end

    -- enter environment
    environment.enter()

    -- has been installed?
    local fetchinfo   = assert(fetch_version(option.get("xmakever")), "cannot fetch xmake version info!")
    local is_official = fetchinfo.is_official
    local mainurls    = fetchinfo.urls
    local version     = fetchinfo.version
    if is_official and xmake.version():eq(version) and not option.get("force") then
        cprint("${bright}xmake %s has been installed!", version)
        return
    end

    local script_only = option.get("scriptonly")

    -- trace
    if is_official then
        cprint("update version ${green}%s${clear} from official source ..", version)
    else
        cprint("update version ${green}%s${clear} from ${underline}%s${clear} ..", version, mainurls[1])
    end

    -- get the temporary source directory without ramdisk (maybe too large)
    local sourcedir = path.join(os.tmpdir({ramdisk = false}), "xmakesrc", version)
    vprint("prepared to download to temp dir %s ..", sourcedir)

    -- all user provided urls are considered as git url since check has been performed in fetch_version
    local install_from_git = not is_official or git.checkurl(mainurls[1])

    -- the download task
    local download_task = function ()
        for idx, url in ipairs(mainurls) do
            tty.erase_line_to_start().cr()
            cprintf("${yellow}  => ${clear}downloading %s .. ", url)
            local ok = try
            {
                function ()
                    os.tryrm(sourcedir)
                    git.clone(url, {depth = 1, recurse_submodules = not script_only, branch = version, outputdir = sourcedir})
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
                cprint("${yellow}  => ${clear}download %s .. ${color.success}${text.success}", url)
                break
            else
                cprint("${yellow}  => ${clear}download %s .. ${color.failure}${text.failure}", url)
            end
            if not ok and idx == #mainurls then
                raise("download failed!")
            end
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

    if install_from_git then
        _check_repo(sourcedir)
    end

    -- do install
    if script_only then
        _install_script(sourcedir)
    else
        _install(sourcedir)
    end
end
