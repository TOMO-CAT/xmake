add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- 本地 repo
add_repositories("my-repo my-repo", {rootdir = os.scriptdir()})
-- 以前的用法: https://github.com/TOMO-CAT/xmake/issues/177
-- 后续 toolchains / options / rules 会默认自动上传, 这个接口只用于 custom scripts 的扩展
-- 具体例子见: example/api/global-interfaces/includes/repo
--
includes("@my-repo/rules/my-repo.mode.release.lua")

-- 远程 repo, 会自动引用其 toolchains / options / rules
add_repositories("cat-repo https://github.com/zxmake/cat-zxmake-repo.git")
-- 以前的用法: https://github.com/TOMO-CAT/xmake/issues/177
-- 后续 toolchains / options / rules 会默认自动上传, 这个接口只用于 custom scripts 的扩展
-- 具体例子见: example/api/global-interfaces/includes/repo
--
includes("@cat-repo/rules/*.lua")

add_rules("my-repo.mode.release")

target("test", function()
    set_kind("binary")
    add_files("src/main.cc")
end)
