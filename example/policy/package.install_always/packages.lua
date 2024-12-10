package("xproto", function()
    set_sourcedir(path.join(os.scriptdir(), "xproto"))
    set_policy("package.install_always", true)

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
end)

package("xcore", function()
    add_deps("xproto")
    set_sourcedir(path.join(os.scriptdir(), "xcore"))
    set_policy("package.install_always", true)

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
end)

package("xmath", function()
    add_deps("xproto", "xcore")
    set_sourcedir(path.join(os.scriptdir(), "xmath"))
    set_policy("package.install_always", true)

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
end)
