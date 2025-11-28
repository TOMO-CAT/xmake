package("xproto", function()
    -- 保证 source embed package 代码改动后能触发增量编译
    set_policy("package.install_always", true)
    set_sourcedir("../xproto")
    add_deps("protobuf-cpp 3.19.4", {system = false})

    on_install(function(package)
        import("package.tools.xmake").install(package)
    end)
end)
