#include "scheduler.h"
#include "globals.h"
#include "utils.h"
#include "pqueue.h"
#include "scenario.h"

/* FCFS: 도착 시각이 빠른 순. 같으면 pid 가 작은 순(먼저 생성된 프로세스). */
int cmp_fcfs(int a, int b) {
    if (scen_proc[a].arrival_time != scen_proc[b].arrival_time)
        return scen_proc[a].arrival_time - scen_proc[b].arrival_time;
    return scen_proc[a].pid - scen_proc[b].pid;
}
int cmp_remaining(int a, int b) {
    if (scen_proc[a].remaining_cpu != scen_proc[b].remaining_cpu)
        return scen_proc[a].remaining_cpu - scen_proc[b].remaining_cpu;
    return scen_proc[a].pid - scen_proc[b].pid;
}
int cmp_priority(int a, int b) {
    if (scen_proc[a].priority != scen_proc[b].priority)
        return scen_proc[a].priority - scen_proc[b].priority;
    return scen_proc[a].pid - scen_proc[b].pid;
}

void tick_run(int running, int* time_quantum){
    scen_proc[running].remaining_cpu--;
    scen_proc[running].executed_cpu++;
    (*time_quantum)--;
}
void tick_interrupt(EVENT* event, bool* is_interrupt){
    int target=event->target_pid;
    event->left_time--;
    scen_proc[target].waiting_time++;
    scen_proc[target].IO_burst_time++;

    if(event->left_time==0){
        event->finished=true;
        scen_proc[target].blocked=false;
        dequeue_wait();
        enqueue_ready(event->target_pid);
        scen_proc[target].event_idx++;

        if(scen_proc[target].event_n==scen_proc[target].event_idx)//더이상 할 인터럽트 없음
            scen_proc[target].event_idx=-1;
        
        (*is_interrupt)=false;
    }
}

void apply_interrupts(EVENT* event,int* ran){ 
    int running = event->target_pid; //==peek
    switch(event->type){
        case IO:    {
            if(*ran>=0) {//이제막 시작된 인터럽트의 경우
                scen_proc[running].blocked = true;
                enqueue_wait(running);
                *ran=-1; //이거땜에 ran 추가로 설정
            }

            break;
        }
        case TIMEOUT:{
            //timeout은 무조건 running>=0
            enqueue_ready(running);
            *ran=-1;
            event->finished=true;
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


void simulate_6(Schedule_Type alg){
    bool is_preemptive=false; int time_quantum=MAX_TIME; bool is_interrupt = false;
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

    for (int t = 0; t < MAX_TIME; t++) {
        /*while(1)으로 감싸진 프로세스 발생처리
            * 프로세스가 동시에 도착할 수 있으므로 while문으로 처리
            * 선점형 알고리즘인 경우, 새로 도착한 프로세스가 실행중인 프로세스보다 우선순위가 높으면 선점 발생
            * 타임 퀀텀 체크, 타임 퀀텀이 다 된 경우 TIMEOUT 인터럽트 발생
            * non-preemptive: 실행중인 프로세스 없을 때만 레디큐에서 프로세스 선택
            * preemptive: 레디큐에 프로세스 있으면 매 틱마다 레디큐에서 가장 우선순위 높은 프로세스 선택
            * 실행중인 프로세스 1 tick 실행
            * 실행중인 프로세스 종료 체크 및 종료 처리*/
         스케줄링처리
            * 타임 퀀텀이 다 되면 TIMEOUT 인터럽트 발생 및 인터럽트 처리 + time_quantum 초기화
            >> RR한정 유의미. 나머지는 time_quantum이 MAX_TIME으로 초기화되어서 타임 퀀텀 체크는 항상 false


        */

        //프로세스 발생처리
        while(1){
            if(proc_time==t){//프로세스 발생
                scen_proc[proc_counted].pid = proc_counted;
                enqueue_ready(scen_proc[proc_counted].pid);

                proc_counted++;

                //다음프로세스 시간설정 프로세스의 마지막에 도달한다면 proc_time=-1
                if(proc_n>proc_counted) proc_time = scen_proc[proc_counted].arrival_time;
                else proc_time=-1;
            }
        }
        
        //스케줄링처리
        if(ready_fifo.size>0||ready_q.size>0){//레디큐에 건덕지 있음
            if(running>=0){
                if(is_preemptive==true){ //선점형만 진입 가능
                    int candidate = pq_peek(&ready_q); //선점형은 무조건 우선순위큐로 레디큐사용
                    if(pick_func(candidate, running)<0){ //새로 도착한 프로세스가 실행중인 프로세스보다 우선순위가 높으면 선점 발생
                        enqueue_ready(running);
                        running=deq(&ready_q);
                        scen_proc[running].state = RUNNING;
                        time_quantum=TIME_QUANTUM;
                    }
                }
                else if(time_quantum<=0){ //RR만 진입 가능
                    EVENT timeout_event = create_TIMEOUT(&scen_proc[running]);
                    apply_interrupts(&timeout_event);
                    running=dequeue_ready();
                    scen_proc[running].state = RUNNING;
                    time_quantum=TIME_QUANTUM;
                }
            }
            else{
                if(q_peek(&wait_q)==-1){//대기큐에 프로세스 없으므로, block상태아님.
                    running=dequeue_ready(); //선점형이든 비선점형이든 레디큐에서 프로세스 꺼내서 실행
                    scen_proc[running].state = RUNNING;
                    time_quantum=TIME_QUANTUM;
                }
                else{
                    continue;
                    //인터럽트가 진행중이면 대기큐에 무조건 프로세스가 있는 구조니까, 인터럽트 진행중인거다.
                }
            }
        }
        else{
            continue;
            //레디큐에 프로세스가 없음. 여기서 할거는 없음. 나머지는 tick_run이랑 check_terminate에서 처리.
        }

        if(running>=0){ //프로세스
            if(scen_proc[running].executed_cpu==scen_proc[running].events[scen_proc[running].event_idx].start_time){
                is_interrupt=true;
                scen_proc[running].events[scen_proc[running].event_idx].target_pid=running;
                apply_interrupts(&scen_proc[running].events[scen_proc[running].event_idx],&running);
                tick_interrupt(&scen_proc[running].events[scen_proc[running].event_idx], &is_interrupt);
            }
            else{
                tick_run(running, &time_quantum);
            }
        }
        else{
            if(is_interrupt){
                tick_interrupt(&scen_proc[running].events[scen_proc[running].event_idx], &is_interrupt);
            }
            else{
                continue; //놀고있는상태
            }
        }


        check_terminate(&running, t, &time_quantum); //실행중인 프로세스 종료 체크 및 running -1
    }
    finalize_stats(alg);         //결과 계산 및 저장 
}
