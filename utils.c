#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "pqueue.h"

/* ---------------- 유틸 ---------------- */
int rand_range(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

void reset_test(void){
    proc_n = scen_proc_n;
    intr_n = scen_intr_n;

    for (int i = 0; i < proc_n; i++) {
        test_proc[i] = scen_proc[i];
        test_proc[i].remaining_cpu      = test_proc[i].cpu_burst;
        test_proc[i].executed_cpu       = 0;
        test_proc[i].state              = NEW;
        test_proc[i].finished           = false;
        test_proc[i].blocked          = false;
        test_proc[i].completion_time    = 0;
        test_proc[i].turnaround_time    = 0;
        test_proc[i].waiting_time       = 0;
        test_proc[i].total_blocked_time = 0;
    }
    for (int i = 0; i < intr_n; i++) {
        test_intr[i] = scen_intr[i];
    }

    pq_init(&ready_q, NULL);  /* ready_q 는 각 스케줄러가 pq_init 으로 비교함수와 함께 초기화 */    
    pq_init(&wait_q, NULL);   /* 대기 큐 초기화 (ready_q 는 각 스케줄러가 pq_init 으로 비교함수와 함께 초기화) */
}

/* ---------------- 큐 보조 함수 ---------------- */
void enqueue_ready(int idx) {
    pq_push(&ready_q, idx);
    test_proc[idx].state = READY;
}
int dequeue_ready(void) {                       /* 우선순위 큐에서 최우선 프로세스 꺼냄 */
    return pq_pop(&ready_q);                    /* 비었으면 -1 */
}
void enqueue_wait(int idx) {
    pq_push(&wait_q, idx);
    test_proc[idx].state = WAITING;
}
int dequeue_wait(void) {                        /* 대기 큐에서 가장 앞 프로세스 꺼냄 */
    return pq_pop(&wait_q);
}
