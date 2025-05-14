-- xmake l example/api/os.cp/test.lua
function main(opt)
    os.cd(os.scriptdir())
    os.tryrm("output")

    -- 1) 这种做法会丢失 A/1 A/2 的文件夹, 相当于有同名文件夹会直接覆盖
    os.mkdir("output/test1")
    os.cp("A/conf", "output/test1", {rootdir = "A/conf"})
    os.cp("B/conf", "output/test1", {rootdir = "B/conf"})

    -- 2) 同样会丢失 A/1 A/2 的文件夹, 即 rootdir 不影响
    os.mkdir("output/test2")
    os.cp("A/conf", "output/test2")
    os.cp("B/conf", "output/test2")

    -- 3) 这种不会丢失 A/1 A/2 的文件夹, 但是我们需要剔除掉 conf 文件夹, 所以用方法 4
    os.mkdir("output/test3")
    os.vrun("cp -r A/conf output/test3")
    os.vrun("cp -r B/conf output/test3")

    -- 4) linux cp 命令是可以使用通配符 * 的, 但是 xmake os.run 会报错, 所以用方法 5
    -- os.mkdir("output/test4")
    -- os.vrun("cp -r A/conf/* output/test3")
    -- os.vrun("cp -r B/conf/* output/test3")

    -- 5) 这种方式用了 bash -c 执行 linux shell 命令, 实现了功能, 不得已而为之
    os.mkdir("output/test5")
    os.vrun("bash -c 'cp -r A/conf/* output/test5'")
    os.vrun("bash -c 'cp -r B/conf/* output/test5'")
end
