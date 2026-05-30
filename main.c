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
    print_result(FCFS);
    scheduler_6(SJF_NP);
    print_result(SJF_NP);
    scheduler_6(SJF_P);
    print_result(SJF_P);
    scheduler_6(PRIO_NP);
    print_result(PRIO_NP);
    scheduler_6(PRIO);
    print_result(PRIO);
    scheduler_6(RR);
    print_result(RR);

    return 0;
}