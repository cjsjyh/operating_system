proj3 = [
    "alarm-single",
    "alarm-multiple",
    "alarm-simultaneous",
    "alarm-priority",
    "alarm-zero",
    "alarm-negative",
    "priority-change",
    "priority-change-2",
    "priority-fifo",
    "priority-preempt",
    "priority-sema",
    "priority-aging",
    "priority-lifo",
    "mlfqs-load-1",
    "mlfqs-load-60",
    "mlfqs-load-avg",
    "mlfqs-recent-1",
    "mlfqs-fair-2",
    "mlfqs-fair-20",
    "mlfqs-nice-2",
    "mlfqs-nice-10",
    "mlfqs-block"
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
for check in proj3:
    found = False
    for result, p_f in results:
        #print("Comparing ",check," ",result)
        if(check == result):
            found = True
            if(p_f != "pass"):
                print("FAILED: ", result)
                is_all_pass = False
    if not found:
        print("NOT FOUND: ", check)

if (is_all_pass):
    print("***CONGRATULATIONS! ALL PASS!***")

