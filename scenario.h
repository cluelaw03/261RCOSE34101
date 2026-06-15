#ifndef SCENARIO_H
#define SCENARIO_H

#include "types.h"

int gen_unique_real_pid();
void create_senario(void);
void create_one_process(int arrival_time, int priority, int i);
void create_IOs(Process* proc);
void create_IO(Process* proc);
void process_interrupt_check(void);
//EVENT create_TIMEOUT(Process* proc);

#endif /* SCENARIO_H */