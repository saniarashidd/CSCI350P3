//Sania Rashid - should implement a workload that demonstrates how priority boost can improve
//performance of a long running CPU bound process..

#include "types.h"
#include "stat.h"
#include "user.h"

int 
main(void) {
    int pid;
    for (int i = 0; i < 9; i++) {
        pid = fork();
        if (pid == 0) { //child
            int x = 1;
            for (int j = 1; j < 200000000; j++) {
                x = (x%j) + 1;
            }
            printf(1, "result: %d\n", x);
            getpinfo(getpid());
            exit();
        }
    }
    //parent waits
    for (int i = 0; i < 9; i++) {
        wait();
    }
    getpinfo(getpid());
    exit();
}