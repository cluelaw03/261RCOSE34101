#ifndef SCENARIO_H
#define SCENARIO_H

#include "types.h"

void create_senario(void);
void create_one_process(int arrival_time, int priority, int i);
void create_interrupts(Process* proc);
void create_interrupt(Process* proc);
void process_interrupt_check(void);

#endif /* SCENARIO_H */