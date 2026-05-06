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
-- @author      OpportunityLiu, ruki
-- @file        fetch_version.lua
--

-- imports
import("core.base.semver")
import("core.base.option")
import("core.base.task")
import("net.http")
import("devel.git")
import("net.fasturl")

-- check if a string looks like a git commit hash (7-40 hex chars)
function _is_commit(str)
    return str and str:match("^[0-9a-fA-F]+$") and #str >= 7 and #str <= 40
end

-- get version and url of provided xmakever
function main(xmakever)

    -- init xmakever
    xmakever = xmakever or "latest"

    -- parse url and commit
    -- * custom_url
    --   * `xmake update -v github:TOMO-CAT/xmake#master` 解析出来 `github:TOMO-CAT/xmake`
    --   * `xmake update -v https://github.com/TOMO-CAT/xmake.git#dev` 解析出来 `https://github.com/TOMO-CAT/xmake.git`
    local commitish = nil
    local custom_url = nil
    local seg = xmakever:split('#', { plain = true, limit = 2, strict = true })
    if #seg == 2 then
        if #seg[1] ~= 0 then
            custom_url = seg[1]
        end
        commitish = seg[2]
    else
        if xmakever:find(':', 1, true) then
            custom_url = xmakever
        else
            commitish = xmakever
        end
    end

    local urls = nil
    if custom_url then
        urls = { git.asgiturl(custom_url) or custom_url }
        cprint("${dim}[fetch_version]${clear} using custom source: ${green}%s${clear}", urls[1])
    else
        if xmake.gitsource() == "github.com" then
            urls = {"https://github.com/TOMO-CAT/xmake.git"}
        else
            urls = {"https://gitee.com/tomocat/xmake.git"}
        end
    end
    commitish = (commitish and #commitish > 0) and commitish or "latest"
    cprint("${dim}[fetch_version]${clear} url: ${green}%s${clear}, commitish: ${green}%s${clear}, runtime: ${green}%s${clear}",
        urls[1], commitish, option.get("runtime") or "luajit")

    -- sort urls
    if #urls > 1 then
        fasturl.add(urls)
        urls = fasturl.sort(urls)
    end

    -- if commitish looks like a git commit hash, use it directly
    if _is_commit(commitish) then
        return {url = urls[1], version = commitish, is_commit = true}
    end

    -- get version
    local version = nil
    local tags, branches
    local matched_url = urls[1]
    for _, url in ipairs(urls) do
        tags, branches = git.refs(url)
        if option.get("verbose") then
            cprint("${dim}[fetch_version]${clear} git.refs url: ${green}%s${clear}, tags: ${green}%s${clear}, branches: ${green}%s${clear}",
                url, table.concat(tags or {}, ", "), table.concat(branches or {}, ", "))
        end
        if tags or branches then
            matched_url = url
            version = semver.select(commitish, tags or {}, tags or {}, branches or {})
            break
        end
    end
    return {url = matched_url, version = (version or "master"), tags = tags, branches = branches}
end
