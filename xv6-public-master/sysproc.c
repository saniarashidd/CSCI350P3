#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h" // Added by Neta Shubin

// Declare ptable as extern so sysproc.c can access it from proc.c - Neta Shubin
extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Added by Neta Shubin
// This routine takes as input an ID of a process (pid) and prints some basic scheduling information about the process to the terminal: the ticks at which it is scheduled, how long it runs before it gives up the CPU, what priority it has each time it is scheduled, etc. This function returns -1 in case of error and 0 in case of success. 
// References slide 31
int
sys_getpinfo(void)
{
  int pid;
  struct proc *p;

  // Return -1 if the argument is invalid
  if(argint(0, &pid) < 0)
    return -1;

  // Get lock
  acquire(&ptable.lock);
  // Get current process With for loop through all processes in ptable
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    // Find pic
    if(p->pid == pid){
      cprintf("name=%s, pid=%d\n", p->name, p->pid);
      cprintf("wait_time=%d\n", p->wait_time);
      cprintf("ticks={%d, %d, %d}\n", p->ticks[0], p->ticks[1], p->ticks[2]);
      cprintf("times={%d, %d, %d}\n", p->times[0], p->times[1], p->times[2]);
      
      // Get start_tick, duration, and priority from each valid item in sched_stats
      for(int i = 0; i < p->num_stats_used; i++){
        cprintf("start=%d, duration=%d, priority=%d\n", 
                p->sched_stats[i].start_tick, 
                p->sched_stats[i].duration, 
                p->sched_stats[i].priority);
      }
      // Release the lock before returning
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1; // Process not found
}
