-- define module: mastercache
local mastercache  = mastercache or {}
local _instance = _instance or {}

-- load modules
local table   = require("base/table")
local io      = require("base/io")
local os      = require("base/os")
local path    = require("base/path")
local global  = require("base/global")

-- new an instance
function _instance.new(name)
    local instance = table.inherit(_instance)
    instance._NAME = name
    instance:load()
    instance._DATA = instance._DATA or {}
    return instance
end

-- get cache name
function _instance:name()
    return self._NAME
end

-- get cache data
function _instance:data()
    return self._DATA
end

-- load cache
function _instance:load()
    -- make sure that the cache directory environment exists
    self._MASTER_PROJECT_CACHE_DIR = os.getenv("XMAKE_MASTER_PROJECT_CACHE_DIR")
    assert(self._MASTER_PROJECT_CACHE_DIR)
    local result = io.load(path.join(self._MASTER_PROJECT_CACHE_DIR, self:name()))
    if result ~= nil then
        self._DATA = result
    end
end

-- save cache
function _instance:save()
    local ok, errors = io.save(path.join(self._MASTER_PROJECT_CACHE_DIR, self:name()), self._DATA)
    if not ok then
        os.raise(errors)
    end
end

-- get cache value in level/1
function _instance:get(key)
    return self._DATA[key]
end

-- get cache value in level/2
function _instance:get2(key1, key2)
    local value1 = self:get(key1)
    if value1 ~= nil then
        return value1[key2]
    end
end

-- get cache value in level/3
function _instance:get3(key1, key2, key3)
    local value2 = self:get2(key1)
    if value2 ~= nil then
        return value2[key3]
    end
end

-- set cache value in level/1
function _instance:set(key, value)
    self._DATA[key] = value
end

-- set cache value in level/2
function _instance:set2(key1, key2, value2)
    local value1 = self:get(key1)
    if value1 == nil then
        value1 = {}
        self:set(key1, value1)
    end
    value1[key2] = value2
end

-- set cache value in level/3
function _instance:set3(key1, key2, key3, value3)
    local value2 = self:get2(key1, key2)
    if value2 == nil then
        value2 = {}
        self:set2(key1, key2, value2)
    end
    value2[key3] = value3
end

-- clear cache scopes
function _instance:clear()
    self._DATA = {}
end

-- get cache instance
function mastercache.cache(cachename)
    local caches = mastercache._CACHES
    if not caches then
        caches = {}
        mastercache._CACHES = caches
    end
    local instance = caches[cachename]
    if not instance then
        instance = _instance.new(cachename)
        caches[cachename] = instance
    end
    return instance
end

-- get all caches
function mastercache.caches(opt)
    opt = opt or {}
    if opt.flush then
        for _, cachefile in ipairs(os.files(path.join(global.cachedir(), "*"))) do
            local cachename = path.filename(cachefile)
            mastercache.cache(cachename)
        end
    end
    return mastercache._CACHES
end

-- get cache value in level/1
function mastercache.get(cachename, key)
    return mastercache.cache(cachename):get(key)
end

-- get cache value in level/2
function mastercache.get2(cachename, key1, key2)
    return mastercache.cache(cachename):get2(key1, key2)
end

-- get cache value in level/3
function mastercache.get3(cachename, key1, key2, key3)
    return mastercache.cache(cachename):get3(key1, key2, key3)
end

-- set cache value in level/1
function mastercache.set(cachename, key, value)
    return mastercache.cache(cachename):set(key, value)
end

-- set cache value in level/2
function mastercache.set2(cachename, key1, key2, value)
    return mastercache.cache(cachename):set2(key1, key2, value)
end

-- set cache value in level/3
function mastercache.set3(cachename, key1, key2, key3, value)
    return mastercache.cache(cachename):set3(key1, key2, key3, value)
end

-- save the given cache, it will save all caches if cache name is nil
function mastercache.save(cachename)
    if cachename then
        mastercache.cache(cachename):save()
    else
        local caches = mastercache.caches()
        if caches then
            for _, cache in pairs(caches) do
                cache:save()
            end
        end
    end
end

-- clear the given cache, it will clear all caches if cache name is nil
function mastercache.clear(cachename)
    if cachename then
        mastercache.cache(cachename):clear()
    else
        local caches = mastercache.caches({flush = true})
        if caches then
            for _, cache in pairs(caches) do
                cache:clear()
            end
        end
    end
end

-- return module: mastercache
return mastercache
