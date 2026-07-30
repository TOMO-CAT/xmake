package("package_with_failed_dep", function()
    add_versions("1.0.0", "0000000000000000000000000000000000000000000000000000000000000000")
    add_deps("package_with_failed_install 1.0.0")

    on_install(function(package)
        os.mkdir(package:installdir("include"))
        io.writefile(path.join(package:installdir(), "include", "package_with_failed_dep.h"), "#pragma once\n")
    end)
end)
