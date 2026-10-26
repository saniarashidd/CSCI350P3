Modified files include:


* Makefile: 
* proc.h: 
* pstat.h: 
* proc.c: updated scheduler() function to implement MFQ priority logic. Instead of the xv6 vanilla round-robin loop over ptable.proc, the scheduler goes through each of the queues: q0, q1, and then q2, in order of priority, then running the highest-priority RUNNABLE process. Scheduling stats are written and demotion rules are applied. boost() was also implemented, where it boosts processes that have been waiting for a long time up in priority.