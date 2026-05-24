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
-- @file        statistics.lua
--

-- imports
import("core.base.option")
import("core.base.process")
import("core.project.config")
import("lib.detect.find_tool")
import("utils.ci.is_running", {alias = "ci_is_running"})
import("private.action.update.fetch_version")

-- grafana cloud config (xor-obfuscated)
local _OBFUSCATE_KEY       = "xmake_stats_key!"

-- loki config
local _LOKI_PUSH_URL_ENC   = "1019151b16655c5b0d1b142c46150b4e1c40515855711406001212310a4b17440c420d040e365c15111d5c295a4a09540b05"
local _LOKI_USERNAME_ENC   = "495b53595d6d40"
local _LOKI_API_TOKEN_ENC  = "1f01023400263902281e1c3626311a153639065e2a1c3a0728194736240c335b1c2a27010426430c2f1e2a6e26213413343a09182907170d002c2133273d1155213a15072907391802334b362726335331070e020007170d2f47326e0a0f1a11372a585b2a1b2906003329112432485b373b0a5e2c36041d03273a690e1c335831070e02061739023b374237082648541a5e2b5b0418251c0247222b262633181e3c5c56"

-- prometheus (influx line protocol) config
local _PROM_PUSH_URL_ENC   = "1019151b16655c5b11061c320e1111440d1e4c1b17301759554d5e2f190a1d0c191d4c050a2d071c0415002b465557460a0c070a0b3e5d1a04005c3e1b0c56574942111e16375c1d0f121f2a134a0e53111904"
local _PROM_USERNAME_ENC   = "4b5f545f576645"

local function _xor_decrypt(hex_str, key)
    local result = {}
    local klen = #key
    for i = 1, #hex_str, 2 do
        local byte = tonumber(hex_str:sub(i, i + 1), 16)
        local ki = ((i - 1) / 2) % klen + 1
        local p, c = 1, 0
        local a, b = byte, key:byte(ki)
        while a > 0 or b > 0 do
            local ra, rb = a % 2, b % 2
            if ra ~= rb then c = c + p end
            a, b, p = (a - ra) / 2, (b - rb) / 2, p * 2
        end
        table.insert(result, string.char(c))
    end
    return table.concat(result)
end

-- loki credentials
local LOKI_PUSH_URL  = _xor_decrypt(_LOKI_PUSH_URL_ENC, _OBFUSCATE_KEY)
local LOKI_USERNAME  = _xor_decrypt(_LOKI_USERNAME_ENC, _OBFUSCATE_KEY)
local LOKI_API_TOKEN = _xor_decrypt(_LOKI_API_TOKEN_ENC, _OBFUSCATE_KEY)

-- prometheus credentials (reuse same api token)
local PROM_PUSH_URL  = _xor_decrypt(_PROM_PUSH_URL_ENC, _OBFUSCATE_KEY)
local PROM_USERNAME  = _xor_decrypt(_PROM_USERNAME_ENC, _OBFUSCATE_KEY)
local PROM_API_TOKEN = LOKI_API_TOKEN

-- statistics is enabled?
function _is_enabled()

    -- disable statistics? need not post it
    local stats = (os.getenv("XMAKE_STATISTICS") or ""):lower()
    if stats == "false" then
        return false
    end

    -- is running on ci(travis/appveyor/...)? need not post it
    -- if ci_is_running() then
    --     os.setenv("XMAKE_STATISTICS", "false")
    --     return false
    -- end
    return true
end

-- post statistics info and only post once everyday when building each project
function post()
    -- get the project directory name
    local projectname = path.basename(os.projectdir())

    -- has been posted today or statistics is disable?
    local outputdir = path.join(os.tmpdir(), "stats", os.date("%y%m%d"), projectname)
    local markfile  = outputdir .. ".mark"
    if os.isdir(outputdir) or os.isfile(markfile) or not _is_enabled() then
        return
    end

    -- mark as posted first, avoid posting it repeatedly
    io.writefile(markfile, "ok")

    -- init argument list
    local argv = {"lua", path.join(os.scriptdir(), "statistics.lua")}
    for _, name in ipairs({"root", "file", "diagnosis", "verbose", "quiet", "yes", "confirm"}) do
        local value = option.get(name)
        if type(value) == "string" then
            table.insert(argv, "--" .. name .. "=" .. value)
        elseif value then
            table.insert(argv, "--" .. name)
        end
    end

    -- pass plat/arch to subprocess via envs, so it doesn't need config.load()
    os.setenv("XMAKE_STATISTICS_PLAT", config.plat() or os.host())
    os.setenv("XMAKE_STATISTICS_ARCH", config.arch() or os.arch())

    -- try to post it in background (detach = true)
    --
    -- xmake lua "/usr/local/share/xmake/modules/private/utils/statistics.lua"
    try
    {
        function ()
            local logfile = path.join(os.tmpdir(), projectname .. ".stats.log")
            process.openv(os.programfile(), argv, {stdout = logfile, stderr = logfile, detach = true, curdir = os.tmpdir()}):close()
        end
    }
end

-- the main function
function main()

    -- push stats to grafana cloud
    local curl = find_tool("curl")
    if curl then
        local timestamp_ns = tostring(os.time()) .. "000000000"
        local host = os.host()
        local plat = os.getenv("XMAKE_STATISTICS_PLAT") or host
        local arch = os.getenv("XMAKE_STATISTICS_ARCH") or os.arch()
        local version = xmake.version():shortstr()
        local backend = "prometheus"

        if backend == "loki" then
            -- push log to grafana cloud loki
            try
            {
                function ()
                    local payload = string.format(
                        '{"streams":[{"stream":{"service_name":"xmake","job":"xmake_build","host":"%s","plat":"%s","arch":"%s","version":"%s"},"values":[["%s","build"]]}]}',
                        host, plat, arch, version, timestamp_ns)
                    print("post loki log: %s", payload)
                    local outdata = os.iorunv(curl.program, {
                        "-sSL", "-X", "POST",
                        "--max-time", "10",
                        "-w", "\nhttp_code=%{http_code}, time_total=%{time_total}s",
                        "-u", LOKI_USERNAME .. ":" .. LOKI_API_TOKEN,
                        "-H", "Content-Type: application/json",
                        "-d", payload,
                        LOKI_PUSH_URL
                    })
                    print("response: %s", outdata)
                end,
                catch
                {
                    function (errors)
                        print("loki error: %s", tostring(errors))
                    end
                }
            }
        else
            -- push metric to grafana cloud prometheus via influx line protocol
            try
            {
                function ()
                    local payload = string.format(
                        "xmake_build,host=%s,plat=%s,arch=%s,version=%s value=1 %s",
                        host, plat, arch, version, timestamp_ns)
                    print("post metric: %s", payload)
                    local outdata = os.iorunv(curl.program, {
                        "-sSL", "-X", "POST",
                        "--max-time", "10",
                        "-w", "\nhttp_code=%{http_code}, time_total=%{time_total}s",
                        "-u", PROM_USERNAME .. ":" .. PROM_API_TOKEN,
                        "-H", "Content-Type: text/plain",
                        "-d", payload,
                        PROM_PUSH_URL
                    })
                    print("response: %s", outdata)
                end,
                catch
                {
                    function (errors)
                        print("prometheus error: %s", tostring(errors))
                    end
                }
            }
        end
    end

    -- fetch the latest version
    -- local versionfile = path.join(os.tmpdir(), "latest_version")
    -- if not os.isfile(versionfile) then
    --     local fetchinfo = try { function () return fetch_version() end }
    --     if fetchinfo then
    --         io.save(versionfile, fetchinfo)
    --     end
    -- end
end
