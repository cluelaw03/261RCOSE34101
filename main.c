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

    simulate_6(FCFS);
    print_result(FCFS);
    simulate_6(SJF_NP);
    print_result(SJF_NP);
    simulate_6(SJF_P);
    print_result(SJF_P);
    simulate_6(PRIO_NP);
    print_result(PRIO_NP);
    simulate_6(PRIO);
    print_result(PRIO);
    simulate_6(RR);
    print_result(RR);

    return 0;
}