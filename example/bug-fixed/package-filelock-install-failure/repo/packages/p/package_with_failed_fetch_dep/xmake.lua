package("package_with_failed_fetch_dep", function()
    add_versions("1.0.0", "0000000000000000000000000000000000000000000000000000000000000000")
    add_deps("package_with_failed_fetch 1.0.0")

    on_fetch(function(package)
        return {version = package:version_str(), includedirs = os.scriptdir()}
    end)
end)
