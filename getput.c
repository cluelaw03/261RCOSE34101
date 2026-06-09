#include "getput.h"
#include <stdio.h>

void print_gantt(void) {
    int starts[MAX_GANTT], cells[MAX_GANTT], seg = 0;
    int i = 0;
    while (i < gantt_n) {                       /* 연속 동일 셀을 구간으로 묶음 */
        int j = i;
        while (j < gantt_n && gantt_pid[j] == gantt_pid[i]) j++;
        starts[seg] = i;
        cells[seg]  = gantt_pid[i];
        seg++;
        i = j;
    }

    printf("\n[Gantt Chart] (cells of length 1)\n");
    for (int s = 0; s < seg; s++) {
        char buf[16];
        int c = cells[s];
        if (c == -1)       snprintf(buf, sizeof(buf), "idle");            /* 유휴 */
        else if (c <= -2)  snprintf(buf, sizeof(buf), "P%d-IO", -c - 2);  /* IO 진행 */
        else               snprintf(buf, sizeof(buf), "P%d", c);          /* CPU 실행 */

        int len = ((s + 1 < seg) ? starts[s + 1] : gantt_n) - starts[s];
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
    if (algo == RR) printf("  (Time Quantum = %d)", cfg.time_quantum);
    printf("\n============================================================\n");

    print_gantt();

    printf("------------------------------------------------------------------------\n");

    printf("\n Completed : %d / %d\n", result_done[algo], result_total[algo]);
    printf(" Avg Waiting    (completed only) : %.2f\n", result_wait[algo]);
    printf(" Avg Turnaround (completed only) : %.2f\n", result_turn[algo]);
}