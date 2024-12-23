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
-- @file        rc.lua
--

-- imports
import("core.base.option")
import("core.base.hashset")
import("core.project.project")

-- normailize path of a dependecy
function _normailize_dep(dep, projectdir)
    if path.is_absolute(dep) then
        dep = path.translate(dep)
    else
        dep = path.absolute(dep, projectdir)
    end
    if dep:startswith(projectdir) then
        return path.relative(dep, projectdir)
    else
        return deps
    end
end

-- parse include file
function _parse_includefile(line)
    if line:startswith("#line") then
        return line:match("#line %d+ \"(.+)\"")
    elseif line:find("ICON", 1, true) and line:find(".ico", 1, true) then
        -- 101 ICON "xxx.ico"
        return line:match("ICON%s+\"(.+.ico)\"")
    elseif line:find("BITMAP", 1, true) and line:find(".bmp", 1, true) then
        return line:match("BITMAP%s+\"(.+.bmp)\"")
    end
end

-- init it
function init(self)
    if self:has_flags("-nologo", "mrcflags") then
        -- fix vs2008 on xp, e.g. fatal error RC1106: invalid option: -ologo
        self:set("mrcflags", "-nologo")
    end
end

-- make the define flag
function nf_define(self, macro)
    return {"-D" .. macro}
end

-- make the undefine flag
function nf_undefine(self, macro)
    return "-U" .. macro
end

-- make the includedir flag
function nf_includedir(self, dir)
    return {"-I" .. dir}
end

-- make the sysincludedir flag
function nf_sysincludedir(self, dir)
    return nf_includedir(self, dir)
end

-- make the compile arguments list
function compargv(self, sourcefile, objectfile, flags)
    return self:program(), table.join(flags, "-Fo" .. objectfile, sourcefile)
end

-- compile the source file
function compile(self, sourcefile, objectfile, dependinfo, flags)

    -- ensure the object directory
    os.mkdir(path.directory(objectfile))

    -- compile it
    try
    {
        function ()
            local program, argv = compargv(self, sourcefile, objectfile, flags)
            local outdata, errdata = os.iorunv(program, argv, {envs = self:runenvs()})
            return (outdata or "") .. (errdata or "")
        end,
        catch
        {
            function (errors)
                if type(errors) == "table" then
                    local errs = errors.stdout or ""
                    if #errs:trim() == 0 then
                        errs = errors.stderr or ""
                    end
                    errors = errs
                end
                os.raise(tostring(errors))
            end
        },
        finally
        {
            function (ok, warnings)
                if warnings and #warnings > 0 and option.get("verbose") then
                    cprint("${color.warning}%s", table.concat(table.slice(warnings:split('\n'), 1, 8), '\n'))
                end
            end
        }
    }
end
