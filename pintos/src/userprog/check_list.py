proj1 = [
    "args-none",
    "args-single",
    "args-multiple",
    "args-many",
    "args-dbl-space",
    "exec-once",
    "exec-multiple",
    "exec-arg",
    "wait-simple",
    "wait-twice",
    "multi-recurse",
    "exit",
    "halt",
    "exec-bad-ptr",
    "exec-missing",
    "sc-bad-arg",
    "sc-bad-sp",
    "sc-boundary",
    "sc-boundary-2",
    "wait-bad-pid",
    "wait-killed"
]
proj2 = [
"args-none",
"args-single",
"args-multiple",
"args-many",
"args-dbl-space",
"sc-bad-sp",
"sc-bad-arg",
"sc-boundary",
"sc-boundary-2",
"sc-boundary-3",
"halt",
"exit",
"create-normal",
"create-empty",
"create-null",
"create-bad-ptr",
"create-long",
"create-exists",
"create-bound",
"open-normal",
"open-missing",
"open-boundary",
"open-empty",
"open-null",
"open-bad-ptr",
"open-twice",
"close-normal",
"close-twice",
"close-stdin",
"close-stdout",
"close-bad-fd",
"read-normal",
"read-bad-ptr",
"read-boundary",
"read-zero",
"read-stdout",
"read-bad-fd",
"write-normal",
"write-bad-ptr",
"write-boundary",
"write-zero",
"write-stdin",
"write-bad-fd",
"exec-once",
"exec-arg",
"exec-bound",
"exec-bound-2",
"exec-bound-3",
"exec-multiple",
"exec-missing",
"exec-bad-ptr",
"wait-simple",
"wait-twice",
"wait-killed",
"wait-bad-pid",
"multi-recurse",
"multi-child-fd",
"rox-simple",
"rox-child",
"rox-multichild",
"bad-read",
"bad-write",
"bad-read2",
"bad-write2",
"bad-jump",
"bad-jump2",
"no-vm/multi-oom",
"lg-create",
"lg-full",
"lg-random",
"lg-seq-block",
"lg-seq-random",
"sm-create",
"sm-full",
"sm-random",
"sm-seq-block",
"sm-seq-random",
"syn-read",
"syn-remove",
"syn-write"
]

fp = open("list.txt","r")

results = []
while True:
    line = fp.readline()
    if not line: break

    p_f = line.strip().split(' ')
    test_case = p_f[1].split('/')[-1]
    results.append((test_case.strip(), p_f[0].strip()))
fp.close()

is_all_pass = True
for check in proj2:
    found = False
    for result, p_f in results:
        #print("Comparing ",check," ",result)
        if(check == result):
            found = True
            if(p_f != "pass"):
                print("FAILED: ", result)
                is_all_pass = False
    if not found:
        print("NOT FOUND: ", test_case)

if (is_all_pass):
    print("***CONGRATULATIONS! ALL PASS!***")

