function main(t)
    -- rust 不支持绝对路径链接静态库, 在和 c++ rule 组合的时候会有问题, 我们先关闭这个单测
    -- if is_host("macosx") and os.arch() ~= "arm64" then
    --     t:build()
    -- else
    --     -- linux 上也可以跑, 但是需要先安装 rust 环境, 暂时先不打开
    --     -- t:build()
    --     return t:skip("wrong host platform")
    -- end
    return t:skip("wrong host platform")
end
