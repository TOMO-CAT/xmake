import("core.project.config")
import("core.platform.platform")
import("core.tool.toolchain")
import("lib.detect.find_tool")

function test_compile_commands(t)
    local projname = "testproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create project
    os.vrunv("xmake", {"create", projname})
    os.cd(projname)

    -- generate compile_commands
    os.vrunv("xmake", {"project", "-k", "compile_commands"})

    -- test autoupdate
    io.insert("xmake.lua", 1, 'add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode", lsp = "clangd"})')
    os.vrun("xmake")

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end

function test_cmake(t)
    local cmake = find_tool("cmake")
    if not cmake then
        return t:skip("cmake not found")
    end
    local projname = "testproj"
    local tempdir = os.tmpfile()
    os.mkdir(tempdir)
    os.cd(tempdir)

    -- create project
    os.vrunv("xmake", {"create", projname})
    os.cd(projname)

    -- generate compile_commands
    os.vrunv("xmake", {"project", "-k", "cmake"})

    -- test build
    os.mkdir("build")
    os.cd("build")
    os.vrunv(cmake.program, {".."})
    os.vrunv(cmake.program, {"--build", "."})

    -- clean up
    os.cd(os.scriptdir())
    os.tryrm(tempdir)
end
