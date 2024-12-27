#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utest/utest.h"

UTEST(main, Fib_10) {
    FILE *fp = popen("echo 10 | ./risc-z fib.bin | tail -n 1", "r");
    fflush(fp);
    char output[64];
    fgets(output, 64, fp);
    pclose(fp);
    ASSERT_STREQ("55\n", output);
}

UTEST_MAIN()
