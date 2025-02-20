package("xmath", function()
    -- 保证 source embed package 代码改动后能触发增量编译
    set_policy("package.install_always", true)
    set_sourcedir("../xmath")
    add_deps("xcore")

    on_install(function(package)
        import("package.tools.xmake").install(package)
    end)
end)
