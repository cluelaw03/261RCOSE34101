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

    //전역구조체 변수접근
    cfg.cpu_burst_min    = 8;   cfg.cpu_burst_max    = 18;
    cfg.normal_burst_min = 5;   cfg.normal_burst_max = 12;
    cfg.io_burst_min     = 5;   cfg.io_burst_max     = 9;
    cfg.time_quantum     = 3;

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

    simulate_with_aging_priority();
    print_result(AGING);
    simulate_with_multibound();
    print_result(MULTIBOUND);

    return 0;
}