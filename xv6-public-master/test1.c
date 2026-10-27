//Sania Rashid - should implement a mixed workload that includes an IO intensive process and a CPU
//intensive process.

#include "types.h"
#include "stat.h"
#include "user.h"

int 
main(void) {
    int pid = fork();
    if (pid > 0) { //parent I/O process
        for (int i = 0; i < 1000; i++) {
            printf(1, "IO output %d\n", i);
        }
        wait();
        getpinfo(getpid());
    } else { //child CPU process
        int x = 0;
        for (int i = 0; i < 10000; i++) {
            x = x + i * i;
        }
        printf(1, "result: %d\n", x);
        getpinfo(getpid());
        exit();
    }
    exit();
}