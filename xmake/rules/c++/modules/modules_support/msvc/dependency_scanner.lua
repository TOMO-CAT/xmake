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
-- @author      ruki, Arthapz
-- @file        msvc/dependency_scanner.lua
--

-- imports
import("core.base.json")
import("core.base.semver")
import("core.project.depend")
import("private.tools.vstool")
import("utils.progress")
import("compiler_support")
import("builder")
import(".dependency_scanner", {inherit = true})

-- generate dependency files
function generate_dependency_for(target, sourcefile, opt)
    raise("deprecated, going to delete msvc later")
    return changed
end
