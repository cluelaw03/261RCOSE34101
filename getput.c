#include "getput.h"
#include <stdio.h>

#include <string.h>

#define GW         4    /* 한 틱당 칸 너비 (2자리 pid + 시각 여백) */
#define GANTT_WRAP 30   /* 한 줄에 표시할 틱(시간) 수 → 줄폭 = GW*WRAP+1 */

/* 셀 라벨: 프로세스=pid, IO='I', idle='.' */
static void gantt_label(int c, char* buf, int n){
    if(c == -1)      snprintf(buf, n, ".");
    else if(c <= -2) snprintf(buf, n, "I");   /* IO: 바로 앞 프로세스가 대상 (단일코어) */
    else             snprintf(buf, n, "%d", c);
}

void print_gantt(void) {
    if(gantt_n <= 0){ printf("\n[Gantt Chart] (empty)\n"); return; }

    printf("\n[Gantt Chart]  (cell = 1 time unit,  pid=프로세스,  I=IO,  .=idle)\n");

    for(int base = 0; base < gantt_n; base += GANTT_WRAP){
        int end = base + GANTT_WRAP;
        if(end > gantt_n) end = gantt_n;

        /* --- 막대 줄 --- */
        for(int t = base; t < end; ){
            int c = gantt_pid[t];
            int run = 1;
            while(t + run < end && gantt_pid[t + run] == c) run++;   /* 연속 구간 묶기 */

            char lab[16]; gantt_label(c, lab, sizeof lab);
            int width = run * GW - 1;                 /* 이 구간 내부 너비 */
            int L = (int)strlen(lab);
            int padl = (width - L) / 2; if(padl < 0) padl = 0;

            putchar('|');
            for(int i = 0; i < padl; i++) putchar(' ');
            fputs(lab, stdout);
            for(int i = padl + L; i < width; i++) putchar(' ');
            t += run;
        }
        putchar('|'); putchar('\n');

        /* --- 경계 시각 줄 (막대 바로 아래, 같은 칸 너비로 정렬) --- */
        for(int t = base; t < end; ){
            int c = gantt_pid[t];
            int run = 1;
            while(t + run < end && gantt_pid[t + run] == c) run++;

            char num[16]; snprintf(num, sizeof num, "%d", t);
            int field = run * GW;                     /* '|' + 내부너비 */
            fputs(num, stdout);
            for(int i = (int)strlen(num); i < field; i++) putchar(' ');
            t += run;
        }
        printf("%d\n\n", end);                        /* 줄 끝 시각 */
    }
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