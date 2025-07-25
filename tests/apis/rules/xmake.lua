
-- define rule: markdown
rule("markdown", function()
    set_extensions(".md", ".markdown")
    on_load(function (target)
        print("markdown: on_load")
    end)
    on_build_file(function (target, sourcefile)
        print("compile %s", sourcefile)
        os.cp(sourcefile, path.join(target:targetdir(), path.basename(sourcefile) .. ".html"))
    end)
end)

-- define rule: man
rule("man", function()
    add_imports("core.project.rule")
    on_build_files(function (target, sourcefiles)
        for _, sourcefile in ipairs(sourcefiles) do
            print("generating man: %s", sourcefile)
        end
    end)
end)

-- define rule: c code
rule("c code", function()
    add_imports("core.tool.compiler")
    before_build_file(function (target, sourcefile)
        print("before_build_file: ", sourcefile)
    end)
    on_build_file(function (target, sourcefile, opt)
        import("core.theme.theme")
        import("utils.progress")
        progress.show(opt.progress, "compiling.$(mode) %s", sourcefile)
        local objectfile_o = os.tmpfile() .. ".o"
        local sourcefile_c = os.tmpfile() .. ".c"
        os.cp(sourcefile, sourcefile_c)
        compiler.compile(sourcefile_c, objectfile_o)
        table.insert(target:objectfiles(), objectfile_o)
    end)
    after_build_file(function (target, sourcefile)
        print("after_build_file: ", sourcefile)
    end)
end)

-- define rule: stub3
rule("stub3", function()
    add_deps("markdown")
    on_load(function (target)
        print("rule(stub3): on_load")
    end)
end)

-- define rule: stub2
rule("stub2", function()
    add_deps("stub3")
    on_load(function (target)
        print("rule(stub2): on_load")
    end)
    before_build(function (target)
        print("rule(stub2): before_build")
    end)
    after_build(function (target)
        print("rule(stub2): after_build")
    end)
    before_build_files(function (target)
        print("rule(stub2): before_build_files")
    end)
    after_build_files(function (target)
        print("rule(stub2): after_build_files")
    end)
end)

-- define rule: stub1
rule("stub1", function()
    add_deps("stub2")
    on_load(function (target)
        print("rule(stub1): on_load")
    end)

    before_build(function (target)
        print("rule(stub1): before_build")
    end)
    after_build(function (target)
        print("rule(stub1): after_build")
    end)

    before_clean(function (target)
        print("rule(stub1): before_clean")
    end)
    after_clean(function (target)
        print("rule(stub1): after_clean")
    end)

    before_install(function (target)
        print("rule(stub1): before_install")
    end)
    on_install(function (target)
        print("rule(stub1): on_install")
    end)
    after_install(function (target)
        print("rule(stub1): after_install")
    end)

    before_uninstall(function (target)
        print("rule(stub1): before_uninstall")
    end)
    on_uninstall(function (target)
        print("rule(stub1): on_uninstall")
    end)
    after_uninstall(function (target)
        print("rule(stub1): after_uninstall")
    end)

    before_package(function (target)
        print("rule(stub1): before_package")
    end)
    on_package(function (target)
        print("rule(stub1): on_package")
    end)
    after_package(function (target)
        print("rule(stub1): after_package")
    end)

    before_run(function (target)
        print("rule(stub1): before_run")
    end)
    on_run(function (target)
        print("rule(stub1): on_run")
    end)
    after_run(function (target)
        print("rule(stub1): after_run")
    end)
end)

-- define rule: stub0b
rule("stub0b", function()
    before_build_file(function (target, sourcefile)
        print("rule(stub0b): before_build_file", sourcefile)
    end)
end)

-- define rule: stub0a
rule("stub0a", function()
    after_build_file(function (target, sourcefile)
        print("rule(stub0a): after_build_file", sourcefile)
    end)
end)

-- define target
target("test", function()

    -- set kind
    set_kind("binary")

    -- add rules
    add_rules("stub1")

    -- add files
    add_files("src/*.c|main2.c", {rules = {"stub0a", "stub0b"}})
    add_files("src/main2.c", {rules = {"stub0a", "stub0b", override = true}})
    add_files("src/man/*.in",   {rule = "man"})
    add_files("src/index.md")
    add_files("src/test.c.in",  {rule = "c code"})

    before_build(function (target)
        print("target: before_build")
    end)

    after_build(function (target)
        print("target: after_build")
    end)
end)
