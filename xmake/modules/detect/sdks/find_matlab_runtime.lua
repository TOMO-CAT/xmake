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
-- @author      WubiCookie
-- @file        find_matlab_runtime.lua
--

-- imports
import("detect.sdks.matlab")

-- find matlab runtime sdk
--
-- @return          the matlab runtime sdk. e.g. {sdkdir = ..., includedirs = ..., linkdirs = ..., .. }
--
-- @code
--
-- local sdk = find_matlab_runtime(opt)
--
-- @endcode
--
function main(opt)
    opt = opt or {}
    local version = opt.require_version and tostring(opt.require_version) or nil
    local result = {sdkdir = "", includedirs = {}, linkdirs = {}, links = {}, bindirs = {}}
    return result
end
