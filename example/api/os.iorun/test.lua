-- xmake l example/api/os.iorun/test.lua
function main(opt)
    local output = os.iorun([[bash -c "cat /etc/passwd | grep '/bin/bash' | wc -l | tr -d '\n'"]])
    print("match lines count: [" .. output .. "]")
end
