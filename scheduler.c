#include <stddef.h>
#include "scheduler.h"
#include "globals.h"
#include "utils.h"
#include "pqueue.h"
#include "scenario.h"
#include <stdio.h>

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
/*
void tick_interrupt(EVENT* event, bool* is_interrupt){
    int target=event->target_pid;
    event->left_time--;
    scen_proc[target].total_blocked_time++;
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
*/
void check_terminate(int *running, int t) {
    if ((*running) < 0) return;
    Process* now_process=&scen_proc[*running];
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
    for (int i = 0; i < scen_proc_n; i++) {
        if (scen_proc[i].finished) {
            total_wait += scen_proc[i].waiting_time;
            total_turn += scen_proc[i].turnaround_time;
            done++;
        }
    }
    result_done[algo]  = done;
    result_total[algo] = scen_proc_n;
    result_wait[algo]  = done ? total_wait / done : 0.0;
    result_turn[algo]  = done ? total_turn / done : 0.0;
}


void simulate_6(Schedule_Type alg){
    bool is_preemptive = false;
    int  time_quantum  = MAX_TIME;
    int (*pick_func)(int,int);

    switch(alg) {
        case FCFS:    pick_func = cmp_fcfs;                                  break;
        case SJF_NP:  pick_func = cmp_remaining;                            break;
        case SJF_P:   is_preemptive = true; pick_func = cmp_remaining;      break;
        case PRIO_NP: pick_func = cmp_priority;                            break;
        case PRIO:    is_preemptive = true; pick_func = cmp_priority;       break;
        case RR:      pick_func = cmp_fcfs; time_quantum = cfg.time_quantum; break;
        default:      pick_func = cmp_fcfs;                                  break;
    }

    reset_test();
    is_ready_fifo = ((alg == RR) || (alg == FCFS));
    q_init(&ready_fifo);
    pq_init(&ready_q, pick_func);
    q_init(&wait_q);

    int running = -1;
    int proc_counted = 0;
    int proc_time = (scen_proc_n > 0) ? scen_proc[0].arrival_time : -1;

    for (int t = 0; t < MAX_TIME; t++) {

        /* 1) 도착 → 레디큐 */
        while(proc_time == t){
            scen_proc[proc_counted].pid = proc_counted;
            enqueue_ready(proc_counted);
            proc_counted++;
            proc_time = (proc_counted < scen_proc_n) ? scen_proc[proc_counted].arrival_time : -1;
        }

        /* 2) RR 타임아웃: 퀀텀 소진 시 레디큐 뒤로 */
        if(alg == RR && running >= 0 && time_quantum <= 0){
            enqueue_ready(running);
            running = -1;
        }

        /* 3) 선점(SJF_P, PRIO): 레디큐 최우선이 실행중보다 우선이면 교체 */
        if(is_preemptive && running >= 0 && !ready_empty()){
            int candidate = pq_peek(&ready_q);
            if(candidate >= 0 && pick_func(candidate, running) < 0){
                enqueue_ready(running);
                running = -1;
            }
        }

        /* 4) 디스패치 + IO 시점 프로세스는 대기큐로 보내고 다음 작업 확보 (CPU 항상 바쁘게) */
        bool runnable = false;
        while(!runnable){
            if(running < 0){
                if(ready_empty()) break;          /* 레디큐 빔 → 이번 틱 idle */
                running = dequeue_ready();
                scen_proc[running].state = RUNNING;
                time_quantum = (alg == RR) ? cfg.time_quantum : MAX_TIME;
            }
            Process* p = &scen_proc[running];
            bool has_event = (p->event_idx >= 0 && p->events != NULL);
            if(has_event && p->executed_cpu == p->events[p->event_idx].start_time){
                p->state = WAITING;               /* IO 발동 → 대기큐로, CPU 즉시 해제 */
                running = -1;
            } else {
                runnable = true;
            }
        }

        /* 5) CPU 1틱 실행 */
        if(running >= 0) tick_run(running, &time_quantum);

        /* 6) 간트 기록 (CPU 점유만; IO 는 병렬 진행이라 표기하지 않음) */
        if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = (running >= 0) ? running : -1;

        /* 7) 모든 WAITING 프로세스 IO 1틱 병렬 진행, 완료 시 레디큐 복귀 */
        for(int i = 0; i < scen_proc_n; i++){
            if(scen_proc[i].state != WAITING) continue;
            if(scen_proc[i].event_idx < 0 || scen_proc[i].events == NULL) continue;
            EVENT* ev = &scen_proc[i].events[scen_proc[i].event_idx];
            ev->left_time--;
            scen_proc[i].total_blocked_time++;
            scen_proc[i].IO_burst_time++;
            if(ev->left_time == 0){
                scen_proc[i].event_idx++;
                if(scen_proc[i].event_idx == scen_proc[i].event_n) scen_proc[i].event_idx = -1;
                enqueue_ready(i);
            }
        }

        /* 8) 종료 처리 */
        check_terminate(&running, t);
    }
    while(gantt_n > 0 && gantt_pid[gantt_n - 1] == -1) gantt_n--;
    finalize_stats(alg);
}

//큐안에 들어있는동안 우선순위 변동으로 내부 변동 반영해야해서 기존의 pq사용불가
//priority를 기준으로 뽑되, 같을경우 pid가 낮은것(먼저 발생했던 프로세스)고르기
static int pick_ready_aging(void){
    int best = -1;
    for(int i=0;i<scen_proc_n;i++){
        if(scen_proc[i].state != READY) continue;
        if(best < 0
           || scen_proc[i].cur_priority <  scen_proc[best].cur_priority
           || (scen_proc[i].cur_priority == scen_proc[best].cur_priority
               && scen_proc[i].pid < scen_proc[best].pid))
            best = i;
    }
    return best;
}

void simulate_with_aging_priority(void){
    reset_test();
    for(int i=0;i<scen_proc_n;i++){          /* reset_test 가 못한 aging 전용 초기화 */
        scen_proc[i].cur_priority = scen_proc[i].priority;
        scen_proc[i].age = 0;
    }
    int running = -1;
    int proc_counted = 0;
    int proc_time = (scen_proc_n > 0) ? scen_proc[0].arrival_time : -1;

    for(int t = 0; t < MAX_TIME; t++){

        /* 1) 도착 → READY */
        while(proc_time == t){
            scen_proc[proc_counted].pid = proc_counted;
            scen_proc[proc_counted].state = READY;
            proc_counted++;
            proc_time = (proc_counted < scen_proc_n) ? scen_proc[proc_counted].arrival_time : -1;
        }

        /* 2) Aging: READY 대기 프로세스 나이 증가 → 임계치마다 우선순위 상승 */
        for(int i = 0; i < scen_proc_n; i++){
            if(scen_proc[i].state == READY && i != running){
                scen_proc[i].age++;
                if(scen_proc[i].age >= AGING_INTERVAL){
                    scen_proc[i].age = 0;
                    if(scen_proc[i].cur_priority > PRIO_MIN) scen_proc[i].cur_priority--;
                }
            }
        }

        /* 3) 선점: READY 최우선이 실행중보다 우선이면 교체 */
        if(running >= 0){
            int best = pick_ready_aging();
            if(best >= 0 && best != running
               && scen_proc[best].cur_priority < scen_proc[running].cur_priority){
                scen_proc[running].state = READY;
                running = -1;
            }
        }

        /* 4) 디스패치 + IO 시점 프로세스는 대기로 보내고 다음 작업 확보 (CPU 항상 바쁘게) */
        bool runnable = false;
        while(!runnable){
            if(running < 0){
                int best = pick_ready_aging();
                if(best < 0) break;                 /* READY 없음 → 이번 틱 idle */
                running = best;
                scen_proc[running].state = RUNNING;
                scen_proc[running].age = 0;
            }
            Process* p = &scen_proc[running];
            bool has_event = (p->event_idx >= 0 && p->events != NULL);
            if(has_event && p->executed_cpu == p->events[p->event_idx].start_time){
                p->state = WAITING;                 /* IO 발동 → 대기, CPU 즉시 해제 */
                running = -1;
            } else {
                runnable = true;
            }
        }

        /* 5) CPU 1틱 (aging 은 퀀텀 없음) */
        if(running >= 0){ int tq = MAX_TIME; tick_run(running, &tq); }

        /* 6) 간트 기록 (CPU 점유만; IO 는 병렬 진행이라 표기하지 않음) */
        if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = (running >= 0) ? running : -1;

        /* 7) 모든 WAITING 프로세스 IO 1틱 병렬 진행, 완료 시 READY 복귀 */
        for(int i = 0; i < scen_proc_n; i++){
            if(scen_proc[i].state != WAITING) continue;
            if(scen_proc[i].event_idx < 0 || scen_proc[i].events == NULL) continue;
            EVENT* ev = &scen_proc[i].events[scen_proc[i].event_idx];
            ev->left_time--;
            scen_proc[i].total_blocked_time++;
            scen_proc[i].IO_burst_time++;
            if(ev->left_time == 0){
                scen_proc[i].event_idx++;
                if(scen_proc[i].event_idx == scen_proc[i].event_n) scen_proc[i].event_idx = -1;
                scen_proc[i].state = READY;
            }
        }

        /* 8) 종료 처리 */
        check_terminate(&running, t);
    }
    while(gantt_n > 0 && gantt_pid[gantt_n - 1] == -1) gantt_n--;
    finalize_stats(AGING);
}

/* === 추가기능 2) Multibound 멀티레벨 큐 === */
static int mb_side_of(int idx){ return (scen_proc[idx].type == IO_Bound) ? 0 : 1; }  /* Normal은 CPU큐로 가정 */
static void mb_enqueue(int idx){
    if(mb_side_of(idx) == 0) q_push(&rq_io, idx); else q_push(&rq_cpu, idx);
    scen_proc[idx].state = READY;
}
static int mb_dispatch(int pref){
    int got;
    if(pref == 0){ got = q_pop(&rq_io);  if(got < 0) got = q_pop(&rq_cpu); }
    else         { got = q_pop(&rq_cpu); if(got < 0) got = q_pop(&rq_io);  }
    return got;
}

void simulate_with_multibound(void){ //비동기식으로 스케줄링 되기떄문에 앞의 예시처럼 인터럽트에 의한 상태와 running을 둘 다 고려할 필요없이 running위주로 고려
    reset_test();
    q_init(&rq_io); q_init(&rq_cpu); //2개 레디큐 준비
    int running = -1, proc_counted = 0;  //기본 파라미터 설정
    int proc_time = (scen_proc_n > 0) ? scen_proc[0].arrival_time : -1;
    int cur_side = 1, time_quantum = TIME_QUANTUM;

    for(int t = 0; t < MAX_TIME; t++){
        while(proc_time == t){ //특이점으로는 enque할때 어디 큐에 넣을지 결정해야함
            scen_proc[proc_counted].pid = proc_counted; mb_enqueue(proc_counted); proc_counted++;
            proc_time = (proc_counted < scen_proc_n) ? scen_proc[proc_counted].arrival_time : -1; //프로세스 발생할거 더 없음처리 -1
        }
    //cpu bound에서 타임 퀀텀 다됨 cpu는 RR이기떄문
        if(running >= 0 && mb_side_of(running) == 1 && time_quantum <= 0){
            q_push(&rq_cpu, running); scen_proc[running].state = READY; running = -1; cur_side ^= 1;
        }
        bool runnable = false;

        //스케줄링
        while(!runnable){ //runnable이 참이되면 break=cpu점유중인 프로세스 인터럽트 안일어나고 진행
            if(running < 0){
                running = mb_dispatch(cur_side); //2개의 큐중 값 뽑기
                if(running < 0) break; //전부 비면  break
                scen_proc[running].state = RUNNING; time_quantum = TIME_QUANTUM;
            }
            Process* p = &scen_proc[running];
            bool has_event = (p->event_idx >= 0 && p->events != NULL);
            if(has_event && p->executed_cpu == p->events[p->event_idx].start_time){
                p->state = WAITING; running = -1; cur_side ^= 1;
            } else runnable = true;
        }
        //cpu작업및 시간 지남 가정
        if(running >= 0) tick_run(running, &time_quantum);
        if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = (running >= 0) ? running : -1;
        for(int i = 0; i < scen_proc_n; i++){
            if(scen_proc[i].state != WAITING) continue;
            if(scen_proc[i].event_idx < 0 || scen_proc[i].events == NULL) continue;
            EVENT* ev = &scen_proc[i].events[scen_proc[i].event_idx];
            ev->left_time--; scen_proc[i].total_blocked_time++; scen_proc[i].IO_burst_time++;
            if(ev->left_time == 0){
                scen_proc[i].event_idx++; if(scen_proc[i].event_idx == scen_proc[i].event_n) scen_proc[i].event_idx = -1;
                mb_enqueue(i);
            }
        }
        int prev = running;
        check_terminate(&running, t);
        if(prev >= 0 && running < 0) cur_side ^= 1;
    }
    while(gantt_n > 0 && gantt_pid[gantt_n - 1] == -1) gantt_n--;
    finalize_stats(MULTIBOUND);
}
