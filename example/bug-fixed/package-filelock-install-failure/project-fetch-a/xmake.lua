add_repositories("filelock-test " .. path.join(os.scriptdir(), "..", "repo"))

add_requires("package_with_failed_fetch_dep 1.0.0", "package_with_slow_fetch 1.0.0", {system = false})

target("filelock", function()
    set_kind("phony")
    add_packages("package_with_failed_fetch_dep", "package_with_slow_fetch")
end)
