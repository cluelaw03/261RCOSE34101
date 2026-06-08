#include <stdlib.h>
#include "utils.h"
#include "globals.h"
#include "pqueue.h"

/* ---------------- 유틸 ---------------- */
int rand_range(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

void reset_test(void){ 
    for (int i = 0; i < proc_n; i++) {
        scen_proc[i].state = NEW;
        scen_proc[i].remaining_cpu      = scen_proc[i].cpu_burst;
        scen_proc[i].executed_cpu       = 0;
        scen_proc[i].total_blocked_time = 0;
        scen_proc[i].finished           = false;
        scen_proc[i].blocked            = false;

        scen_proc[i].completion_time    = 0;
        scen_proc[i].turnaround_time    = 0;
        scen_proc[i].waiting_time       = 0;
        scen_proc[i].IO_burst_time       = 0;
        scen_proc[i].event_idx         = (scen_proc[i].event_n > 0) ? 0 : -1;
        for(int j=0; j<scen_proc[i].event_n; j++){
            scen_proc[i].events[j].left_time = scen_proc[i].events[j].duration;
            scen_proc[i].events[j].finished = false;
        }
    }

    gantt_n = 0;
    isblocked=false;
}

/* ---------------- 큐 보조 함수 ---------------- */
void enqueue_ready(int idx) {
    if (is_ready_fifo) {
        q_push(&ready_fifo, idx);
    } else {
        pq_push(&ready_q, idx);
    }
    test_proc[idx].state = READY;
}
int dequeue_ready(void) {                       /* 우선순위 큐에서 최우선 프로세스 꺼냄 */
    if(is_ready_fifo)   return q_pop(&ready_fifo);
    return pq_pop(&ready_q);                    /* 비었으면 -1 */
}
void enqueue_wait(int idx) {
    q_push(&wait_q, idx);
    test_proc[idx].state = WAITING;
}
int dequeue_wait(void) {                        /* 대기 큐에서 가장 앞 프로세스 꺼냄 */
    return q_pop(&wait_q);
}
