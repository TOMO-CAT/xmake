-- regression test for io.readfile / file:read("*a") corruption in text mode
--
-- bug: when reading a > ~128KB pure-text file in text mode, the returned
-- string had the same length but a stretch of bytes near the 128KB boundary
-- was replaced with NUL bytes (or otherwise mangled), because the C-layer
-- native_large_allocator_ralloc lost the trailing sizeof(head) bytes of
-- the user buffer when migrating from native heap to mmap.

local function bytes_to_hex(s, base, span)
    local out = {}
    for i = 1, span do
        local b = s:byte(base + i - 1)
        out[#out + 1] = b and ("%02x"):format(b) or ".."
    end
    return table.concat(out, " ")
end

local function check_one(line_payload_len, line_count)
    local path = os.tmpfile() .. ".txt"
    local payload = ("x"):rep(line_payload_len)
    local lines = {}
    for _ = 1, line_count do
        lines[#lines + 1] = payload
    end
    local content = table.concat(lines, "\n") .. "\n"

    -- write via binary mode to ensure on-disk content is exactly `content`
    local wf = assert(io.open(path, "wb"))
    wf:write(content)
    wf:close()

    -- read via TEXT mode (the historically buggy path)
    local rf = assert(io.open(path, "r"))
    local got = rf:read("*a")
    rf:close()

    os.rm(path)

    if got == content then
        print(("ok: line_len=%d count=%d total=%d"):format(line_payload_len, line_count, #content))
        return true
    end

    -- diagnose
    local n = math.min(#got, #content)
    local first_div, last_div
    for i = 1, n do
        if got:sub(i, i) ~= content:sub(i, i) then
            first_div = i
            break
        end
    end
    if first_div then
        last_div = first_div
        for i = first_div + 1, n do
            if got:sub(i, i) == content:sub(i, i) then break end
            last_div = i
        end
    end
    print(("FAIL: line_len=%d count=%d total=%d, expected_len=%d got_len=%d"):format(
        line_payload_len, line_count, #content, #content, #got))
    if first_div then
        print(("  first divergence at offset %d, run length %d"):format(
            first_div, (last_div or first_div) - first_div + 1))
        print(("  expected @%d: %s"):format(
            first_div - 4, bytes_to_hex(content, first_div - 4, 24)))
        print(("  actual   @%d: %s"):format(
            first_div - 4, bytes_to_hex(got, first_div - 4, 24)))
    end
    return false
end

function main()
    local cases = {
        {40, 3500},   -- ~140 KB, classic reproducer
        {50, 3000},   -- ~150 KB, larger corruption window
        {8,  30000},  -- many short lines, ~270 KB
        {80, 2000},   -- ~160 KB
    }
    local all_ok = true
    for _, c in ipairs(cases) do
        if not check_one(c[1], c[2]) then
            all_ok = false
        end
    end
    if not all_ok then
        os.exit(1)
    end
    print("PASS: io.readfile text-mode large-file regression")
end
