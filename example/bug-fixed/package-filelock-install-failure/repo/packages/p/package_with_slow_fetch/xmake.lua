package("package_with_slow_fetch", function()
    add_versions("1.0.0", "0000000000000000000000000000000000000000000000000000000000000000")

    on_fetch(function(package)
        local marker_dir = assert(os.getenv("FILELOCK_MARKER_DIR"))
        os.mkdir(marker_dir)
        io.writefile(path.join(marker_dir, "slow-fetch-started"), tostring(os.getpid()))
        os.sleep(3000)
        return {version = package:version_str(), includedirs = os.scriptdir()}
    end)
end)
