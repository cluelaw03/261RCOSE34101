#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "scenario.h"
#include "utils.h"
#include "scheduler.h"
#include "getput.h"
#include "pqueue.h"

int main(void) {
    srand(time(NULL));
    create_senario();

    scheduler_6(FCFS);
    print_gantt(FCFS);
    scheduler_6(SJF_NP);
    print_gantt(SJF_NP);
    scheduler_6(SJF_P);
    print_gantt(SJF_P);
    scheduler_6(PRIO_NP);
    print_gantt(PRIO_NP);
    scheduler_6(PRIO);
    print_gantt(PRIO);
    scheduler_6(RR);
    print_gantt(RR);
    
    return 0;
}