# Task

- **Course:** https://hwproj.ru/courses/20022
- **Task:** 2. Overcommit

# Setup

- **OS:** Ubuntu 22.04 LTS
- **Compiler:** GCC 11.4.0
- **RAM:** 32G
- **Swap:** 16G
- **Script:** [run_experiments.sh](run_experiments.sh)

# Result

When `vm.overcommit_memory` is set to `0`, then experiment fails with the following error:
```
Memory allocation failed: Cannot allocate memory
```

When `vm.overcommit_memory` is set to `1`, then RAM and swap are gradually filled up and then the process is killed:
```
Starting experiments
Page 0k out of 26214k
Page 100k out of 26214k
Page 200k out of 26214k
...
Page 8700k out of 26214k
Page 8800k out of 26214k
Page 8900k out of 26214k
./02.overcommit/run_experiments.sh: line 8: 691477 Killed                  ./overcommit 100
```

# Conclusion

Setting `vm.overcommit_memory` to `1` allows to allocate more memory than physically available.
Process is only killed only when enough allocated pages are actually used.
