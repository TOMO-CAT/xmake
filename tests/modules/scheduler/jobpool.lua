import("private.async.jobpool")

local function fake_run_func(jobname)
    return function()
        print(jobname .. " is running")
    end
end

-- xmake l tests/modules/scheduler/jobpool.lua
function main()
    local batchjobs = jobpool.new()

    -- G -> C -> B -> A;
    -- G -> F -> E -> D;
    local rootjob = batchjobs:rootjob()
    local job_g = batchjobs:addjob("G", fake_run_func("G"), { rootjob = rootjob })
    local job_c = batchjobs:addjob("C", fake_run_func("C"), { rootjob = job_g })
    local job_b = batchjobs:addjob("B", fake_run_func("B"), { rootjob = job_c })
    local job_a = batchjobs:addjob("A", fake_run_func("A"), { rootjob = job_b })
    local job_f = batchjobs:addjob("F", fake_run_func("F"), { rootjob = job_g })
    local job_e = batchjobs:addjob("E", fake_run_func("E"), { rootjob = job_f })
    local job_d = batchjobs:addjob("D", fake_run_func("D"), { rootjob = job_e })

[[
{
  "root",
  {
    {
      "F",
      {
        "D",
        "E"
      }
    },
    {
      {
        "B",
        "A"
      },
      "C"
    },
    "G"
  }
}
]]
    print(tostring(batchjobs))
end
