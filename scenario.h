#ifndef SCENARIO_H
#define SCENARIO_H

#include "types.h"

void create_senario(void);
void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i);
void create_interrupt(int start_time, int duration, int target_pid, EVENT_Type type, int i);
void process_interrupt_check(void);

#endif /* SCENARIO_H */