-- xmake l example/api/os.iorun_with_pipes/test.lua
function main(opt)
    -- 可以运行带管道符的复杂 shell 命令
    -- @see https://github.com/TOMO-CAT/xmake/issues/140
    --
    -- 官方实现方法比较复杂: https://github.com/xmake-io/xmake/discussions/6002
    local output = os.iorun_with_pipes(
                       "cat /etc/passwd | grep '/bin/bash' | wc -l | tr -d '\n'")
    print("match lines count: [" .. output .. "]")
end
