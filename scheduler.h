#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
int cmp_fcfs(int a, int b);
int cmp_remaining(int a, int b);
int cmp_priority(int a, int b);
void schedule(Schedule_Type algo);

/* 원본에서 schedule_fcfs 가 호출하는 미구현 함수들 */
void tick_run(int *running, int *intr_running, int *time_quantum);
void advance_interrupts(void);
void check_terminate(int *running, int t);
void finalize_stats(Schedule_Type alg);
void apply_interrupts(Interrupt_Type intr_type, int *running);
void advance_interrupts(void);

#endif /* SCHEDULER_H */
