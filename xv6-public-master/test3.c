//Sania Rashid - should implement a workload that demonstrates how MFQ scheduler can be gamed. A
//process manages to stay in the highest priority queue by yielding the CPU before its time slice expires.
//Hint: you can create a loop that iteratively calls sleep() to relinquish the processor before the time
//interrupt occurs. The number of ticks will not be updated, and the process will stay in the highest priority
//queue.

#include "types.h"
#include "stat.h"
#include "user.h"

int 
main(void) {
    for (int i = 0; i < 200; i++) {
        //printf(1, "hi\n");
        sleep(1);
    }
    getpinfo(getpid());
    exit();
    

}