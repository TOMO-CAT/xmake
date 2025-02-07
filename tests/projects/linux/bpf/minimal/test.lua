function main(t)
    -- 安装 Python-3.12.3.tgz 报错, 暂时跳过
    -- ./configure: line 4644: syntax error near unexpected token `('
    -- ./configure: line 4644: `  printf %s "(cached) " >&6'
    if true then
        return
    end

    if is_host("linux") and os.arch() == "x86_64" then
        os.vrun("xmake f -y -p android -vD")
        os.vrun("xmake -y -vD")
    end
end
