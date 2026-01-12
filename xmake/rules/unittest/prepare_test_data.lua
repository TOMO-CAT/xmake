function main(target, opt)
    -- 只处理 add_testfiles 场景
    if not target:get("testfiles") then
        return
    end

    -- 建立单测二进制软链
    -- @see https://github.com/TOMO-CAT/xmake/pull/222
    os.ln(path.absolute(target:targetfile()), path.join(target:rundir(), path.filename(target:targetfile())))

    -- 拷贝 testfiles 到 rundir
    for _, dst_path in ipairs(table.wrap(target:get("testfiles"))) do
        local extra = target:extraconf("testfiles", dst_path) or {}
        -- 默认使用相同路径
        local src_path = dst_path
        if extra.src_path then
            src_path = extra.src_path
        end
        if path.is_absolute(dst_path) then
            raise(
                "cannot set absolute path [%s] for testfiles destination in target [%s]",
                dst_path, target:name())
        end
        local dst_abs_path = path.absolute(path.join(target:rundir(), dst_path))
        local src_abs_path = path.absolute(src_path)
        os.mkdir(path.directory(dst_abs_path))

        -- 默认用软链, 除非显式指定 softlink 为 false
        if extra.softlink == false then
            -- os.cp(src_abs_path, dst_abs_path)

            -- cp -rT:
            -- * 在 Linux 拷贝中, 重命名文件夹最容易犯的错误是如果 A/foo/bar 已经存在, 再次执行 cp -r m/bar A/foo/bar 会变成 A/foo/bar/bar
            -- * 使用 -T (no-target-directory) 参数强制将 dest 视为普通文件/文件夹名, 如果 dest 已存在，它会直接覆盖或合并而不会在下面创建子目录
            os.vrun(format("bash -c 'cp -rT %s %s'", src_abs_path, dst_abs_path))
        else
            -- os.cp(src_abs_path, dst_abs_path, {symlink = true})
            --  os.ln(src_abs_path, dst_abs_path, {force = true})

            -- cp -asT:
            -- * s: 创建软链接而不是拷贝文件
            -- * -T: 禁止将目标视为目录 (防止在 A/foo/bar 下创建子目录 bar/bar)
            -- * -a: 以“归档模式”创建软链树, 目录属性和结构完全同步
            os.vrun(format("bash -c 'cp -asT %s %s'", src_abs_path, dst_abs_path))
        end
    end
end
