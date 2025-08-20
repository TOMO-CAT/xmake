-- imports
import("core.base.option")
import("lib.detect.find_tool")
import("net.proxy")

function main(opt)
    -- init options
    opt = opt or {}

    -- find git
    local git = assert(find_tool("git"), "git not found!")

    -- init argv
    local argv = {}
    if opt.fsmonitor then
        table.insert(argv, "-c")
        table.insert(argv, "core.fsmonitor=true")
    else
        table.insert(argv, "-c")
        table.insert(argv, "core.fsmonitor=false")
    end
    table.insert(argv, "fetch")

    -- set remote
    table.insert(argv, opt.remote or "origin")

    -- set branch
    if opt.branch then
        table.insert(argv, opt.branch)
    end

    -- use proxy?
    local envs
    local proxy_conf = proxy.config()
    if proxy_conf then
        -- get proxy configuration from the current remote url
        local remoteinfo = try { function() return os.iorunv(git.program, {"remote", "-v"}, {curdir = opt.repodir}) end }
        if remoteinfo then
            for _, line in ipairs(remoteinfo:split('\n', {plain = true})) do
                local splitinfo = line:split("%s+")
                if #splitinfo > 1 and splitinfo[1] == (opt.remote or "origin") then
                    local url = splitinfo[2]
                    if url then
                        proxy_conf = proxy.config(url)
                    end
                    break
                end
            end
        end
        envs = {ALL_PROXY = proxy_conf}
    end

    -- fetch it
    os.vrunv(git.program, argv, {envs = envs, curdir = opt.repodir})
end
