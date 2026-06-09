#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
int cmp_fcfs(int a, int b);
int cmp_remaining(int a, int b);
int cmp_priority(int a, int b);
void simulate_6(Schedule_Type algo);

void simulate_with_aging_priority(void);
void simulate_with_multibound(void);


void tick_run(int running, int* time_quantum);
void tick_interrupt(EVENT* event, bool* is_interrupt);
void check_terminate(int *running, int t);
void finalize_stats(Schedule_Type alg);
void apply_interrupts(EVENT* event, int* ran);

#endif /* SCHEDULER_H */
