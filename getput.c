#include "getput.h"
#include <stdio.h>

void print_gantt(void) {
    int starts[MAX_GANTT], pids[MAX_GANTT], seg = 0;
    int i = 0;
    while (i < gantt_n) {
        int j = i;
        while (j < gantt_n && gantt_pid[j] == gantt_pid[i]) j++;
        starts[seg] = i;
        pids[seg]   = gantt_pid[i];
        seg++;
        i = j;
    }
    printf("\n[Gantt Chart] (cells of length 1)\n");
    for (int s = 0; s < seg; s++) {
        char buf[12];
        if (pids[s] == -1) snprintf(buf, sizeof(buf), "idle");
        else               snprintf(buf, sizeof(buf), "P%d", pids[s]);
        int len = (int)(starts[s+1 < seg ? s+1 : s] - starts[s]);
        if (s == seg - 1) len = gantt_n - starts[s];
        printf("| %s(%d) ", buf, len);
    }
    printf("|\n");
    printf("Timeline: ");
    for (int s = 0; s < seg; s++) printf("%d ", starts[s]);
    printf("%d\n", gantt_n);
}

void print_result(Schedule_Type algo) {
    printf("\n============================================================\n");
    printf(" Algorithm : %s", schedule_name[algo]);
    if (algo == RR) printf("  (Time Quantum = %d)", TIME_QUANTUM);
    printf("\n============================================================\n");

    print_gantt();

    printf("------------------------------------------------------------------------\n");

    printf("\n Completed : %d / %d\n", result_done[algo], result_total[algo]);
    printf(" Avg Waiting    (completed only) : %.2f\n", result_wait[algo]);
    printf(" Avg Turnaround (completed only) : %.2f\n", result_turn[algo]);
}