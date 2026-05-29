#include "scheduler.h"
#include "globals.h"
#include "utils.h"

void schedule_fcfs(void) {
    reset_test();
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
            int pos = pick_fifo();//구현필요 레디큐에서 뽑기
            if (pos >= 0) {
                running = ready_q[pos];
                remove_ready_at(pos);
                test_proc[running].state = RUNNING;
            }
        }
        tick_run(running);              //구현필요 실행중인 프로세스 1 tick 실행
        advance_interrupts();           //구현필요 인터럽트 처리
        check_terminate(&running, t); //구현필요 실행중인 프로세스 종료 체크
    }
    finalize_stats(FCFS);         //구현필요 결과 계산 및 저장 
}