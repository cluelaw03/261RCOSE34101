#include "scheduler.h"
#include "globals.h"
#include "utils.h"
#include "pqueue.h"

/* FCFS: 도착 시각이 빠른 순. 같으면 pid 가 작은 순(먼저 생성된 프로세스). */
static int cmp_fcfs(int a, int b) {
    if (test_proc[a].arrival_time != test_proc[b].arrival_time)
        return test_proc[a].arrival_time - test_proc[b].arrival_time;
    return test_proc[a].pid - test_proc[b].pid;
}

void schedule_fcfs(void) {
    reset_test();
    pq_init(&ready_q, cmp_fcfs);     /* Ready Queue 를 FCFS 기준으로 초기화 */
    int running = -1;
    int proc_queued=0; int intr_queued=0;
    int proc_time = -1; int intr_time = -1;

    if(proc_n>0) proc_time = test_proc[0].arrival_time;
    if(intr_n>0) intr_time = test_intr[0].start_time;

    for (int t = 0; t < MAX_TIME; t++) {
        if(proc_time==t){
            enqueue_ready(test_proc[proc_queued].pid-1);
            proc_queued++;
            if(proc_n>proc_queued) 
                proc_time = test_proc[proc_queued].arrival_time;
        }
        if(intr_time==t){
            //apply_interrupts(t, &running);
            intr_queued++;
               if(intr_n>intr_queued)
                 intr_time = test_intr[intr_queued].start_time;
        } // 프로세스 도착과 인터럽트 발생 체크


        if (running == -1) {                 /* non-preemptive: 비었을 때만 선택 */
            int idx = dequeue_ready();        /* 우선순위 큐에서 최우선 프로세스 꺼냄 */
            if (idx >= 0) {
                running = idx;
                test_proc[running].state = RUNNING;
            }
        }
        tick_run(running);              //구현필요 실행중인 프로세스 1 tick 실행
        advance_interrupts();           //구현필요 인터럽트 처리
        check_terminate(&running, t); //구현필요 실행중인 프로세스 종료 체크
    }
    finalize_stats(FCFS);         //구현필요 결과 계산 및 저장 
}
