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
for check in proj1:
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

