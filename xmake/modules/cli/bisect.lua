-- imports
import("core.base.option")
import("lib.detect.find_tool")

-- the options
local options = {
    {'g', "good",     "kv",  nil, "Set the good commit."},
    {'b', "bad",      "kv",  nil, "Set the bad commit."},
    {nil, "gitdir",   "kv",  nil, "Set the git root directory."},
    {'c', "commands", "kv" , nil, "Run the multiple commands instead of the default build command.",
                                  "e.g.",
                                  "    $ xmake l cli.bisect -c 'xmake -rv' -g good -b bad",
                                  "    $ xmake l cli.bisect -c 'xmake -vD; xmake run hello' -g good -b bad"},
    {'s', "script"  , "kv" , nil, "Run the given lua script file.",
                                  "e.g.",
                                  "    $ xmake l cli.bisect -s /tmp/test.lua -g good -b bad"},
    {'-', "arbitrary", "vs", nil, "Run an arbitrary command.",
                                  "e.g.",
                                  "    $ xmake l cli.bisect -g 90846dd -b ddb86e4 -- xmake -rv"}
}

-- run command
function _run_command(opt)
    opt = opt or {}
    local ok = try {
        function()
            local commands = opt.commands
            local scriptfile = opt.script
            local arbitrary = opt.arbitrary
            if commands then
                for _, command in ipairs(commands:split(";")) do
                    os.exec(command:trim())
                end
            elseif arbitrary then
                local program = arbitrary[1]
                local argv = #arbitrary > 1 and table.slice(arbitrary, 2) or {}
                os.execv(program, argv)
            elseif scriptfile and os.isfile(scriptfile) and
                path.extension(scriptfile) == ".lua" then
                local script = import(path.basename(scriptfile), {
                    rootdir = path.directory(scriptfile),
                    anonymous = true
                })
                script(events)
            end
            return true
        end, catch {
            function(errors)
                cprint(tostring(errors))
            end
        }
    }
    return ok
end

-- use `git bisect` to analyze problem
--
-- eg.
--    $ xmake l cli.bisect --good=v3.0.3 --bad=v3.0.5 --gitdir=/tmp/xmake -c 'xmake --version'
--
-- params.
--    @good: start from this good commit / tag
--    @bad:  end by this bad commit / tag
function main(...)
    local argv = table.pack(...)
    local args = option.parse(argv, options,
                              "Analyze problem using `git bisect`.", "",
                              "Usage: xmake l cli.bisect [options]")

    local git = assert(find_tool("git"), "git not found!")
    local good = assert(args.good, "please set `--good commit`")
    local bad = assert(args.bad, "please set `--bad commit`")
    local gitdir = args.gitdir or os.curdir()

    -- The git bisect command uses a binary search algorithm to quickly locate the commit that introduced the issue.
    os.execv(git.program, {"bisect", "start"}, {curdir = gitdir})
    os.execv(git.program, {"bisect", "good", good}, {curdir = gitdir})
    os.execv(git.program, {"bisect", "bad", bad}, {curdir = gitdir})

    -- Execute the command provided by the user.
    -- If the command runs successfully, mark it as "good".
    -- Otherwise, mark it as "bad".
    -- Continue this process until the first "bad commit" is found.
    while true do
        local output
        if _run_command(args) then
            output = os.iorunv(git.program, {"bisect", "good"},
                               {curdir = gitdir})
        else
            output =
                os.iorunv(git.program, {"bisect", "bad"}, {curdir = gitdir})
        end
        if output then
            print(output)
            if output:find("is the first bad commit", 1, true) then
                break
            end
        end
    end
end
