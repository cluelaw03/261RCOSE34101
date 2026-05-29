#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

void schedule_fcfs(void);

/* 원본에서 schedule_fcfs 가 호출하는 미구현 함수들 */
int  pick_fifo(void);
void tick_run(int running);
void advance_interrupts(void);
void check_terminate(int *running, int t);
void finalize_stats(Schedule_Type alg);

#endif /* SCHEDULER_H */