import("core.package.package", {alias = "core_package"})
import("core.cache.memcache")

local leaf_buildhash = "11111111111111111111111111111111"
local parent_buildhash = "22222222222222222222222222222222"

function _new_package(name, version)
    local packagedir = path.join(os.tmpdir(), "xmake_package_parent_manifest_test", name)
    os.mkdir(packagedir)
    io.writefile(path.join(packagedir, "xmake.lua"), ([[
package("%s")
    set_kind("library")
]]):format(name))
    memcache.clear()
    local package = core_package.load_from_repository(name, packagedir)
    package:version_set(version, "version")
    package:plat_set(os.subhost())
    package:arch_set(os.subarch())
    package._BUILDHASH = name == "fallback_parent" and parent_buildhash or leaf_buildhash
    return package
end

function _install_package_manifest(package, buildhash, manifest)
    local installdir = package:_installdir_with_buildhash(buildhash, {version = manifest.version})
    os.mkdir(path.join(installdir, "include"))
    io.save(path.join(installdir, "manifest.txt"), table.join({
        name = package:name(),
        kind = "library",
        plat = package:plat(),
        arch = package:arch(),
        mode = package:mode(),
        configs = {},
        vars = {
            includedirs = {"include"}
        }
    }, manifest))
end

function _fetch_from_parent_manifest(depversion, require_version)
    local leaf = _new_package("fallback_leaf", require_version)
    local parent = _new_package("fallback_parent", "1.0.0")
    leaf:parents_add(parent)

    _install_package_manifest(leaf, leaf_buildhash, {
        version = depversion
    })
    _install_package_manifest(parent, parent_buildhash, {
        version = "1.0.0",
        librarydeps = {"fallback_leaf"},
        deps = {
            fallback_leaf = {
                version = depversion,
                buildhash = leaf_buildhash
            }
        }
    })
    return leaf:_fetch_from_parent_manifests({require_version = require_version})
end

function test_fetch_from_parent_manifest_with_same_plain_version(t)
    local version = "5-newcommit-newhash"
    local fetchinfo = _fetch_from_parent_manifest(version, version)
    t:require(fetchinfo)
    t:are_equal(fetchinfo.version, version)
end

function test_fetch_from_parent_manifest_skips_mismatched_plain_version(t)
    local fetchinfo = _fetch_from_parent_manifest("5-oldcommit-oldhash", "5-newcommit-newhash")
    t:require(not fetchinfo)
end

function test_fetch_from_parent_manifest_with_semver_range(t)
    local fetchinfo = _fetch_from_parent_manifest("1.2.3", "~1.2.0")
    t:require(fetchinfo)
    t:are_equal(fetchinfo.version, "1.2.3")
end
