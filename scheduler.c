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

void tick_run(int* running, int* time_quantum) {
    Process* now_process=&scen_proc[*running];

    if(*running>=0){
        if(now_process->event_idx==now_process->executed_cpu) //이 시점에 인터럽트 발생
            apply_interrupts(now_process->events[now_process->event_idx], running);
        else{
            now_process->remaining_cpu--; now_process->executed_cpu++; (*time_quantum)--;
            gantt_pid[gantt_n++] = now_process->pid;
        }
    }
    else{
        apply_interrupts(now_process->events[now_process->event_idx], running);
    }
}

void apply_interrupts(EVENT event, int *running){ 
    switch(event.type){
        case IO:    {
            isblocked = true;

            if((*running)>=0){
                enqueue_wait(*running);
                (*running)=-1;
            }

            event.left_time--;
            scen_proc[event.target_pid].IO_burst_time++;
            if(event.left_time==0){
                scen_proc[event.target_pid].event_idx++;
                isblocked=false;
            }
            break;
        }
        case TIMEOUT:{
            if(*running>=0){
                enqueue_ready(*running);
                (*running)=-1;
            }
            break;
        }
        default: break;
   }
}

void check_terminate(int *running, int t, int * time_quantum) {
    Process* now_process=&scen_proc[*running];
    if ((*running) < 0) return;
    if (now_process->remaining_cpu == 0) {
        now_process->state           = TERMINATED;
        now_process->finished        = 1;
        now_process->completion_time = t + 1;
        now_process->turnaround_time = now_process->completion_time - now_process->arrival_time;
        now_process->waiting_time    = now_process->turnaround_time - now_process->cpu_burst - now_process->total_blocked_time;
        (*running) = -1;
        return;
    }
    return;
}

void finalize_stats(Schedule_Type algo) {
    double total_wait = 0, total_turn = 0;
    int    done = 0;
    for (int i = 0; i < proc_n; i++) {
        if (scen_proc[i].finished) {
            total_wait += scen_proc[i].waiting_time;
            total_turn += scen_proc[i].turnaround_time;
            done++;
        }
    }
    result_done[algo]  = done;
    result_total[algo] = proc_n;
    result_wait[algo]  = done ? total_wait / done : 0.0;
    result_turn[algo]  = done ? total_turn / done : 0.0;
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

    //RR은 우선순위 큐를 사용하면 선입선출에 큰 오류 가능
    is_ready_fifo = ((alg == RR)||(alg == FCFS));
    if (is_ready_fifo) q_init(&ready_fifo);
    else               pq_init(&ready_q, pick_func);
    q_init(&wait_q);

    int running = -1;
    int proc_counted=0; int proc_time = -1;

    if(proc_n>0) proc_time = scen_proc[0].arrival_time;

    for (int t = 0; t < MAX_TIME; t++) {// 프로세스 발생 시간과 인터럽트 발생 시간 체크 및 처리

        while(1){//프로세스 발생 시 레디큐에 넣기. 여러 프로세스가 동시에 도착할 수 있으므로 while문으로 처리
            if(proc_time==t){
                if(is_preemptive==true&&running>=0&&(pick_func!=cmp_fcfs)){ //새로 도착한 프로세스가 현재 실행중인 프로세스보다 우선순위가 높으면 선점
                    if(pick_func(scen_proc[proc_counted].pid-1,running)<0){
                        enqueue_ready(running);
                        running=-1;
                    }
                }
                enqueue_ready(scen_proc[proc_counted].pid-1);

                proc_counted++;
                if(proc_n>proc_counted) proc_time = scen_proc[proc_counted].arrival_time;
                else proc_time=-1;
            }
            else break;
        }
        
        if(time_quantum<=0){ //RR 타임 퀀텀 체크, 타임 퀀텀이 다 된 경우 TIMEOUT 인터럽트 발생
            if(is_preemptive==true){
                //apply_interrupts(TIMEOUT, &running, &intr_running);
                running=-1;
            }
            time_quantum=TIME_QUANTUM;
        }
        if (running == -1&&isblocked==false) {                 /* non-preemptive: 비었을 때만 선택 */
            int idx = dequeue_ready();        /* 우선순위 큐에서 최우선 프로세스 꺼냄 */
            if (idx >= 0) {
                running = idx;
                test_proc[running].state = RUNNING;
                if(alg==RR) time_quantum=TIME_QUANTUM;
            }
        }
 
        tick_run(&running, &time_quantum);              //실행중인 프로세스 1 tick 실행
        check_terminate(&running, t, &time_quantum); //실행중인 프로세스 종료 체크 및 running -1
    }
    finalize_stats(alg);         //결과 계산 및 저장 
}
