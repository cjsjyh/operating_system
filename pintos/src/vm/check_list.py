proj4 = [
    "pt-grow-stack",
    "pt-grow-stk-sc",
    "pt-big-stk-obj",
    "pt-grow-pusha",
    "page-linear",
    "page-parallel",
    "page-shuffle",
    "page-merge-seq",
    "page-merge-par",
    "page-merge-mm",
    "page-merge-stk",
    "pt-bad-addr",
    "pt-bad-read",
    "pt-write-code",
    "pt-write-code2",
    "pt-grow-bad"
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
for check in proj4:
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

