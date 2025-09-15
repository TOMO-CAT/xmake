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
-- @file        package.lua
--

-- define module
local package = {}
local _instance = _instance or {}

-- load modules
local io            = require("base/io")
local os            = require("base/os")
local path          = require("base/path")
local table         = require("base/table")
local utils         = require("base/utils")
local semver        = require("base/semver")
local rule          = require("project/rule")
local config        = require("project/config")
local sandbox       = require("sandbox/sandbox")
local localcache    = require("cache/localcache")
local instance_deps = require("base/private/instance_deps")

-- save the requires info to the cache
function _instance:save()
    package._cache():set(self:name(), self._INFO)
    package._cache():save()
end

-- clear the package
function _instance:clear()
    local info = self._INFO
    if info then
        for k, v in pairs(info) do
            if not k:startswith("__") then
                info[k] = nil
            end
        end
    end
    self._COMPONENT_DEPS = nil
    self._LINK_TO_FULLPATH = nil
    self._LINKFLAG_CACHE = {}
end

-- dump this package
function _instance:dump()
    utils.dump(self._INFO)
end

-- get the require info
function _instance:get(infoname)
    return self._INFO[infoname]
end

-- get the package name (with alias name)
function _instance:name()
    return self._NAME
end

-- get the package linkflag caache
function _instance:linkflag_cache()
    return self._LINKFLAG_CACHE
end

-- get the package version
function _instance:version()

    -- get it from cache first
    if self._VERSION ~= nil then
        return self._VERSION
    end

    -- get version
    local version = nil
    local verstr = self:get("version")
    if verstr then
        version = semver.new(verstr)
    end
    self._VERSION = version or false
    return version
end

-- 获取 package version 字符串
function _instance:version_str()
    return tostring(self:version())
end

-- 存储每个 link 及其对应的绝对路径
-- * 静态库返回绝对路径
-- * 未搜索到的静态库或者动态库返回原值
function _instance:link2fullpath()
    -- get it from cache first
    if self._LINK_TO_FULLPATH ~= nil then
        return self._LINK_TO_FULLPATH
    end

    -- 构造 link to path
    local link2path = {}
    for _, link in ipairs(table.wrap(self:get("links"))) do
        -- 默认情况下 (动态库或者没找到) 用 link
        local fullpath = link
        for _, libfile in ipairs(table.wrap(self:libraryfiles())) do
            local libfile_name = path.filename(libfile)
            if libfile_name == "lib" .. link .. ".a" then
                fullpath = libfile
            end
        end
        link2path[link] = fullpath
    end
    self._LINK_TO_FULLPATH = link2path
    return link2path
end


-- 目前是给 target:_get_from_packages 接口调用的, 用于构造 link fullpath 和隐藏 linkdir
-- name 只能是 links 或者 linkdirs
function _instance:get_linkflags(name, opt)
    local opt = opt or {}
    local linkflag_cache = self:linkflag_cache()

    -- 构造 cachekey, 该接口存在性能问题所以需要缓存结果
    local cachekey = self:name() .. "_GET_V2_" .. name
    -- 允许用户通过 add_packages("xxx", {linkpath = false) 来禁用绝对路径
    -- 默认禁用 fullpath (因为存在性能问题)
    -- if opt.linkpath == true then
    --     cachekey = cachekey .. "_WITH_LINKPATH"
    -- end
    if opt.linkpath ~= false then
        cachekey = cachekey .. "_WITHOUT_LINKPATH"
    end
    if opt.values then
        cachekey = cachekey .. "_" .. table.concat(opt.values, "_")
    end

    if linkflag_cache[cachekey] ~= nil then
        return linkflag_cache[cachekey]
    end

    -- 避免 values 为 nil, 否则 cache 会持续无法命中
    local values = opt.values or self:get(name) or {}
    if name == "links" or name == "linkdirs" then
        -- if opt.linkpath then
        if opt.linkpath ~= false then
            if self:only_static_lib() and name == "linkdirs" then
                values = {}
            elseif name == "links" then
                values = self:transform_to_link_fullpath(values)
            end
        end
    end

    -- 写入缓存
    linkflag_cache[cachekey] = values
    return values
end

-- 将 links 转换成绝对路径, 指向性更明确一些
-- * 目前只支持静态库
-- * 动态库使用绝对路径会导致移动二进制后无法运行 (依赖的动态库必须在编译时的绝对路径下)
-- @see https://github.com/xmake-io/xmake/issues/5066
-- @see https://github.com/TOMO-CAT/xmake/issues/189
function _instance:transform_to_link_fullpath(links)
    if not links then
        return nil
    end

    local fullpaths = {}
    local link2fullpath = self:link2fullpath()
    for _, link in ipairs(table.wrap(links)) do
        local fp = link2fullpath[link]
        if fp then
            table.insert(fullpaths, fp)
        else
            -- 有一些 package links 会包含 deps 的 links, 但是 libfiles 却没有, 这里查找不到的话当作系统库 -l 处理
            table.insert(fullpaths, link)
        end
    end
    return fullpaths
end

-- get the package license
function _instance:license()
    return self:get("license")
end

-- has static libraries?
function _instance:has_static()
    return self:get("static")
end

-- has shared libraries?
function _instance:has_shared()
    return self:get("shared")
end

-- get the require string
function _instance:requirestr()
    return self:get("__requirestr")
end

-- get the require configuration from the given name
--
-- e.g.
--
-- add_requires("xxx", {system = true, configs = {shared = true}})
--
-- local configs = pkg:requireconf()
-- local system = pkg:requireconf("system")
-- local shared = pkg:requireconf("configs", "shared")
--
function _instance:requireconf(name, key)
    local requireconfs = self:get("__requireconfs")
    local value = requireconfs
    if name then
        value = requireconfs and requireconfs[name] or nil
        if value and key then
            value = value[key]
        end
    end
    return value
end

-- get the install directory
-- @see https://github.com/xmake-io/xmake/issues/3106
function _instance:installdir()
    return self:get("installdir")
end

-- get library files
function _instance:libraryfiles()
    return self:get("libfiles")
end

-- 判断 package 是否只包含静态库
function _instance:only_static_lib()
    return self:has_static() and (not self:has_shared())
end

-- get components
function _instance:components()
    return self:get("components")
end

-- get default components
function _instance:components_default()
    return self:get("__components_default")
end

-- get components list with link order
function _instance:components_orderlist()
    return self:get("__components_orderlist")
end

-- get the dependencies of components
function _instance:components_deps()
    return self:get("__components_deps")
end

-- get user extra configuration from package/on_fetch
-- @see https://github.com/xmake-io/xmake/issues/3106#issuecomment-1330143922
--
-- e.g.
--
-- @code
-- package("xxx")
--     on_fetch(function (package)
--         return {includedirs = "", links = "", extras = {foo = ""}}
--     end)
--
-- @endcode
--
-- we can also get extra configuration from package/add_xxx
--
-- e.g.
--
-- @code
-- package("xxx")
--     add_linkgroups("foo", {group = true})
--
-- target:pkg("xxx"):extraconf("linkgroups", "foo", "group")
-- @endcode
--
-- extras = {
--     linkgroups = {
--         z = {
--             group = true
--         }
--     }
-- }
--
function _instance:extraconf(name, item, key)
    local extraconfs = self:get("extras")
    if not extraconfs then
        return
    end

    -- get configuration
    local extraconf = extraconfs[name]

    -- get configuration value
    local value = extraconf
    if item then
        value = extraconf and extraconf[item] or nil
        if value == nil and extraconf and type(item) == "table" then
            value = extraconf[table.concat(item, "_")]
        end
        if value and key then
            value = value[key]
        end
    end
    return value
end

-- get order dependencies of the given component
function _instance:component_orderdeps(name)
    local component_orderdeps = self._COMPONENT_ORDERDEPS
    if not component_orderdeps then
        component_orderdeps = {}
        self._COMPONENT_ORDERDEPS = component_orderdeps
    end

    -- expand dependencies
    local orderdeps = component_orderdeps[name]
    if not orderdeps then
        orderdeps = table.reverse_unique(self:_sort_componentdeps(name))
        component_orderdeps[name] = orderdeps
    end
    return orderdeps
end

-- set the value to the requires info
function _instance:set(name_or_info, ...)
    if type(name_or_info) == "string" then
        local args = ...
        if args ~= nil then
            self._INFO[name_or_info] = table.unwrap(table.unique(table.join(...)))
        else
            self._INFO[name_or_info] = nil
        end
    elseif table.is_dictionary(name_or_info) then
        for name, info in pairs(table.join(name_or_info, ...)) do
            self:set(name, info)
        end
    end
end

-- add the value to the requires info
function _instance:add(name_or_info, ...)
    if type(name_or_info) == "string" then
        local info = table.wrap(self._INFO[name_or_info])
        self._INFO[name_or_info] = table.unwrap(table.unique(table.join(info, ...)))
    elseif table.is_dictionary(name_or_info) then
        for name, info in pairs(table.join(name_or_info, ...)) do
            self:add(name, info)
        end
    end
end

-- this require info is enabled?
function _instance:enabled()
    return self:get("__enabled")
end

-- enable or disable this require info
function _instance:enable(enabled)
    self:set("__enabled", enabled)
end

-- get the given rule
function _instance:rule(name)
    return self:rules()[name]
end

-- get package rules
-- @see https://github.com/xmake-io/xmake/issues/2374
function _instance:rules()
    local rules = self._RULES
    if rules == nil then
        local ruleinfos = {}
        local installdir = self:installdir()
        local rulesdir = path.join(installdir, "rules")
        if os.isdir(rulesdir) then
            local files = os.filedirs(path.join(rulesdir, "*"))
            if files then
                for _, filepath in ipairs(files) do
                    local results, errors
                    if filepath:endswith(".lua") then
                        results, errors = rule._load(filepath)
                    elseif os.isdir(filepath) and os.isfile(path.join(filepath, "xmake.lua")) then
                        results, errors = rule._load(path.join(filepath, "xmake.lua"))
                    else
                        os.raise("unknown rule %s: %s", os.isdir(filepath) and "directory" or "file", filepath)
                    end
                    if results then
                        table.join2(ruleinfos, results)
                    else
                        os.raise(errors)
                    end
                end
            end
        end

        -- make rule instances
        rules = {}
        for rulename, ruleinfo in pairs(ruleinfos) do
            rulename = "@" .. self:name() .. "/" .. rulename
            local instance = rule.new(rulename, ruleinfo, {package = self})
            if instance:script("load") then
                utils.warning("we cannot add `on_load()` in package rule(%s), please use `on_config()` instead of it!", rulename)
            end
            if instance:script("load_after") then
                utils.warning("we cannot add `after_load()` in package rule(%s), please use `on_config()` instead of it!", rulename)
            end
            rules[rulename] = instance
        end
        self._RULES = rules
    end
    return rules
end

-- sort component deps
function _instance:_sort_componentdeps(name)
    local orderdeps = {}
    local plaindeps = self:components_deps() and self:components_deps()[name]
    for _, dep in ipairs(table.wrap(plaindeps)) do
        table.insert(orderdeps, dep)
        table.join2(orderdeps, self:_sort_componentdeps(dep))
    end
    return orderdeps
end

-- we need to sort package set keys by this string
-- @see https://github.com/xmake-io/xmake/pull/2971#issuecomment-1290052169
function _instance:__tostring()
    return "<package: " .. self:name() .. ">"
end

-- get cache from `.xmake/${plat}/${arch}/cache/package`
function package._cache()
    return localcache.cache("package")
end

-- load the package from the cache
function package.load(name)
    local info = package._cache():get(name)
    if info == nil then
        return
    end
    return package.load_withinfo(name, info)
end

-- load package from the give package info
function package.load_withinfo(name, info)
    local instance = table.inherit(_instance)
    instance._INFO = info
    instance._NAME = name
    instance._LINKFLAG_CACHE = {}
    return instance
end

-- return module
return package
