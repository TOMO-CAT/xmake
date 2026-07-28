import("core.base.global")
import("core.package.package")

function main(t)
    local cleaner = import("actions.build.hourly_cleaner", {rootdir = os.programdir()})
    local rootdir = os.tmpfile() .. ".dir"
    os.tryrm(rootdir)

    local filelockdir = path.join(rootdir, "filelock")
    local installdir = path.join(rootdir, "packages")
    local cachedir = path.join(rootdir, "cache", "packages")
    global.set("filelockdir", filelockdir, {force = true})
    global.set("pkg_installdir", installdir, {force = true})
    global.set("pkg_cachedir", cachedir, {force = true})
    package._FILELOCKDIR = nil
    package._INSTALLDIR = nil
    package._CACHEDIR = nil
    os.setenv("XMAKE_PKG_RETAIN_HOURS", "1")

    local old_time = os.time() - 8 * 24 * 3600
    local pkg_lockfile = path.join(filelockdir, "packages", "p__pkg__v1.0.lock")
    local repo_lockfile = path.join(filelockdir, "repositories", "repo.lock")
    local pkg_installdir = path.join(installdir, "p", "pkg", "v1.0")
    local pkg_cachedir = path.join(package.cachedir(), "p", "pkg", "v1.0")
    os.mkdir(path.directory(pkg_lockfile))
    os.mkdir(path.directory(repo_lockfile))
    os.mkdir(pkg_installdir)
    os.mkdir(pkg_cachedir)
    io.writefile(pkg_lockfile, "")
    io.writefile(repo_lockfile, "")
    io.save(pkg_lockfile .. ".info", {time = old_time, time_formatted = "old"})
    io.save(repo_lockfile .. ".info", {time = old_time, time_formatted = "old"})

    cleaner.main()

    t:require(not os.isfile(pkg_lockfile))
    t:require(not os.isfile(pkg_lockfile .. ".info"))
    t:require(not os.isdir(pkg_installdir))
    t:require(not os.isdir(pkg_cachedir))
    t:require(not os.isfile(repo_lockfile))
    t:require(not os.isfile(repo_lockfile .. ".info"))

    os.tryrm(rootdir)
end
