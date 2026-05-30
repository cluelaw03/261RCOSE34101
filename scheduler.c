#include "scheduler.h"
#include "globals.h"
#include "utils.h"
#include "pqueue.h"

/* FCFS: 도착 시각이 빠른 순. 같으면 pid 가 작은 순(먼저 생성된 프로세스). */
int cmp_fcfs(int a, int b) {
    if (test_proc[a].arrival_time != test_proc[b].arrival_time)
        return test_proc[a].arrival_time - test_proc[b].arrival_time;
    return test_proc[a].pid - test_proc[b].pid;
}
int cmp_remaining(int a, int b) {
    if (test_proc[a].remaining_cpu != test_proc[b].remaining_cpu)
        return test_proc[a].remaining_cpu - test_proc[b].remaining_cpu;
    return test_proc[a].pid - test_proc[b].pid;
}
int cmp_priority(int a, int b) {
    if (test_proc[a].priority != test_proc[b].priority)
        return test_proc[a].priority - test_proc[b].priority;
    return test_proc[a].pid - test_proc[b].pid;
}

void tick_run(int* running,int* intr_running, int* time_quantum) {
    if (*running >= 0) {
        test_proc[*running].remaining_cpu--;
        test_proc[*running].executed_cpu++;
        (*time_quantum)--;
        gantt_pid[gantt_n++] = test_proc[*running].pid;
    }
    else {
        gantt_pid[gantt_n++] = -1;
    }
    /*
    else{
        test_intr[*intr_running].left_time--;

        if(test_intr[*intr_running].left_time<=0){
            test_intr[*intr_running].left_time=0;
            isblocked=false;
            dequeue_wait();
            test_proc[pq_peek(&wait_q)].state = READY;
            (*intr_running)=-1;
        }
    }
    */
}

void check_terminate(int *running, int t) {
    if ((*running) < 0) return;
    if (test_proc[*running].remaining_cpu == 0) {
        test_proc[*running].state           = TERMINATED;
        test_proc[*running].finished        = 1;
        test_proc[*running].completion_time = t + 1;
        test_proc[*running].turnaround_time = test_proc[*running].completion_time - test_proc[*running].arrival_time;
        test_proc[*running].waiting_time    = test_proc[*running].turnaround_time - test_proc[*running].cpu_burst - test_proc[*running].total_blocked_time;
        (*running) = -1;
        return;
    }
    return;
}

void finalize_stats(Schedule_Type algo) {
    double total_wait = 0, total_turn = 0;
    int    done = 0;
    for (int i = 0; i < proc_n; i++) {
        if (test_proc[i].finished) {
            total_wait += test_proc[i].waiting_time;
            total_turn += test_proc[i].turnaround_time;
            done++;
        }
    }
    result_done[algo]  = done;
    result_total[algo] = proc_n;
    result_wait[algo]  = done ? total_wait / done : 0.0;
    result_turn[algo]  = done ? total_turn / done : 0.0;
}


void apply_interrupts(EVENT_Type event_type, int *running, int *intr_running) {
    switch(event_type){
        case IO:    {
            isblocked = true;

            if((*running)>=0){
                enqueue_wait(*running);
                (*running)=-1;
            }
            break;
        }
        case TIMEOUT:{
            enqueue_ready(*running);
            (*running)=-1;
            break;
        }
        default: break;
   }
}

void scheduler_6(Schedule_Type alg){
    bool is_preemptive=false; int time_quantum=MAX_TIME;
    int (*pick_func)(int,int);

    switch(alg) {
        case FCFS: {
            pick_func = cmp_fcfs;
            break;
        }
        case SJF_NP: {
            pick_func = cmp_remaining;
            break;
        }
        case SJF_P: {
            is_preemptive=true;
            pick_func = cmp_remaining;
            break;
        }
        case PRIO_NP: {
            pick_func = cmp_priority;
            break;
        }
        case PRIO: {
            is_preemptive=true;
            pick_func = cmp_priority;
            break;
        }
        case RR: {
            pick_func = cmp_fcfs;
            is_preemptive=true;
            time_quantum=TIME_QUANTUM;
            break;
        }
        default: break;
    }
    reset_test();
    pq_init(&ready_q, pick_func);     /* Ready Queue 를 초기화 */
    pq_init(&wait_q, cmp_fcfs);         /* Waiting Queue 는 FCFS 로 관리 */
    int running = -1; int intr_running=-1;
    int proc_counted=0; int intr_counted=0;
    int proc_time = -1; int intr_time = -1;

    if(proc_n>0) proc_time = test_proc[0].arrival_time;
    if(intr_n>0) intr_time = test_intr[0].start_time;

    for (int t = 0; t < MAX_TIME; t++) {
        if(time_quantum<=0){
            if(is_preemptive==true){
                apply_interrupts(TIMEOUT, &running, &intr_running);
                running=-1;
            }
            time_quantum=TIME_QUANTUM;
        }
        if(proc_time==t){
            if(is_preemptive==true&&running>=0){
                if(pick_func(test_proc[proc_counted].pid-1,running)<0){
                    enqueue_ready(running);
                    running=-1;
                }
            }
            enqueue_ready(test_proc[proc_counted].pid-1);
            proc_counted++;
            if(proc_n>proc_counted) 
                proc_time = test_proc[proc_counted].arrival_time;
        }
        /*
        if(intr_time==t){
            intr_counted++;
            intr_running=intr_counted;
            apply_interrupts(test_intr[intr_counted].type, &running, &intr_running);
               if(intr_n>intr_counted)
                 intr_time = test_intr[intr_counted].start_time;
        } // 프로세스 도착과 인터럽트 발생 체크
        */
        if (running == -1&&isblocked==false) {                 /* non-preemptive: 비었을 때만 선택 */
            int idx = dequeue_ready();        /* 우선순위 큐에서 최우선 프로세스 꺼냄 */
            if (idx >= 0) {
                running = idx;
                test_proc[running].state = RUNNING;
            }
        }
        tick_run(&running, &intr_running, &time_quantum);              //실행중인 프로세스 1 tick 실행
        check_terminate(&running, t); //실행중인 프로세스 종료 체크 및 running -1
    }
    finalize_stats(alg);         //결과 계산 및 저장 
}
