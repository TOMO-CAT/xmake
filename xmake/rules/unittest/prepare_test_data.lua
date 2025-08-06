function main(target, opt)
    -- 只处理 add_testfiles 场景
    if not target:get("testfiles") then
        return
    end

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
        if extra.softlink == false then
            -- 默认用软链, 除非显式指定 softlink 为 false
            os.cp(src_abs_path, dst_abs_path)
        else
             os.ln(src_abs_path, dst_abs_path, {force = true})

        end
    end
end
