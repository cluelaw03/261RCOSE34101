#include <stddef.h>
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
            time_quantum=cfg.time_quantum;
            break;
        }
        default: break;
    }

    reset_test();

    //RR은 우선순위 큐를 사용하면 선입선출에 큰 오류 가능
    is_ready_fifo = ((alg == RR)||(alg == FCFS));
    q_init(&ready_fifo);              /* 둘 다 초기화하면 직전 실행 잔여 size가 안 남음 */
    pq_init(&ready_q, pick_func);     /* RR/FCFS여도 cmp는 항상 유효(cmp_fcfs)하니 init 안전 */
    q_init(&wait_q);

    int running = -1;  
    int blocked_idx = -1;
    int proc_counted=0; 
    int proc_time = -1;

    if(scen_proc_n>0) proc_time = scen_proc[0].arrival_time;

    for (int t = 0; t < MAX_TIME; t++) {

        //프로세스 발생처리
        while(1){
            if(proc_time==t){//프로세스 발생
                scen_proc[proc_counted].pid = proc_counted;
                enqueue_ready(scen_proc[proc_counted].pid);

                proc_counted++;

                //다음프로세스 시간설정 프로세스의 마지막에 도달한다면 proc_time=-1
                if(scen_proc_n>proc_counted) proc_time = scen_proc[proc_counted].arrival_time;
                else {proc_time=-1; break;}
            }
            else{
                break;
            }
        }
        
        //스케줄링처리
        if(!ready_empty()){ //레디큐에 건덕지 있음
            if(running>=0){
                if(is_preemptive==true){ //선점형만 진입 가능
                    int candidate = pq_peek(&ready_q); //선점형은 무조건 우선순위큐로 레디큐사용
                    if(pick_func(candidate, running)<0){ //새로 도착한 프로세스가 실행중인 프로세스보다 우선순위가 높으면 선점 발생
                        enqueue_ready(running);
                        running=dequeue_ready();
                        scen_proc[running].state = RUNNING;
                        time_quantum = (alg==RR) ? cfg.time_quantum : MAX_TIME;
                    }
                }
                else if(alg==RR && time_quantum<=0){ //RR만 진입 가능
                    EVENT timeout_event = create_TIMEOUT(&scen_proc[running]);
                    apply_interrupts(&timeout_event, &running);
                    running=dequeue_ready();
                    scen_proc[running].state = RUNNING;
                    time_quantum = (alg==RR) ? cfg.time_quantum : MAX_TIME;
                }
            }
            else{
                if(q_peek(&wait_q)==-1){//대기큐에 프로세스 없으므로, block상태아님.
                    running=dequeue_ready(); //선점형이든 비선점형이든 레디큐에서 프로세스 꺼내서 실행
                    scen_proc[running].state = RUNNING;
                    time_quantum = (alg==RR) ? cfg.time_quantum : MAX_TIME;
                }
                //else{ IO 진행 } block상태
            }
        }
        //else{레디큐에 프로세스가 없음. 여기서 할거는 없음. 나머지는 tick_run이랑 check_terminate에서 처리.}

        if(running>=0){ //프로세스
            Process* p = &scen_proc[running];
            bool has_event = (p->event_idx >= 0 && p->events != NULL);

            if(has_event && p->executed_cpu == p->events[p->event_idx].start_time){
                is_interrupt=true;
                p->events[p->event_idx].target_pid = running;
                blocked_idx = running;
                apply_interrupts(&p->events[p->event_idx], &running);
                
                int io_idx = blocked_idx;                 // 이 틱은 io_idx 의 IO
                Process* bp = &scen_proc[io_idx];
                tick_interrupt(&bp->events[bp->event_idx], &is_interrupt);
                if(!is_interrupt) blocked_idx = -1;         // IO 끝나면 비움, io가 1짜리인경우
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -(io_idx + 2); //간트기록
            }
            else{
                tick_run(running, &time_quantum);
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = running;   //간트기록
            }
        }
        else{
            if(is_interrupt){
                int io_idx = blocked_idx;
                Process* bp = &scen_proc[io_idx];
                tick_interrupt(&bp->events[bp->event_idx], &is_interrupt);
                if(!is_interrupt) blocked_idx = -1;          // IO 끝나면 비움 
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -(io_idx + 2);
            }
            else{
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -1;               // idle 셀
                continue;
            }
        }


        check_terminate(&running, t); //실행중인 프로세스 종료 체크 및 running -1
    }
    while(gantt_n > 0 && gantt_pid[gantt_n - 1] == -1) gantt_n--;  // 후행 idle 제거
    finalize_stats(alg);         //결과 계산 및 저장 
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
    for(int i=0;i<scen_proc_n;i++){//reset_test에서 못한 초기화 마저하기==> 기존과의 차이점
        scen_proc[i].cur_priority = scen_proc[i].priority;
        scen_proc[i].age = 0;
    }
    //초기 지역변수 파라미터 설정
    int running = -1, blocked_idx = -1;
    bool is_interrupt = false;
    int proc_counted = 0;
    int proc_time = (scen_proc_n > 0) ? scen_proc[0].arrival_time : -1;


    for(int t = 0; t < MAX_TIME; t++){
        while(proc_time == t){//기존이랑 동일
            scen_proc[proc_counted].pid = proc_counted;
            scen_proc[proc_counted].state = READY;
            proc_counted++;
            proc_time = (proc_counted < scen_proc_n) ? scen_proc[proc_counted].arrival_time : -1; 
        }
        for(int i = 0; i < scen_proc_n; i++){ //레디큐에서 기다리고 있을 프로세스에 에이징처리용 파라미터 업데이트 및 일정이상 파라미터 되면 우선순위 높이기
            if(scen_proc[i].state == READY && i != running){
                scen_proc[i].age++;
                if(scen_proc[i].age >= AGING_INTERVAL){
                    scen_proc[i].age = 0;
                    if(scen_proc[i].cur_priority > PRIO_MIN) scen_proc[i].cur_priority--;
                }
            }
        }

        //스케줄링처리
        if(!is_interrupt){
            int best = pick_ready_aging(); 
            if(running < 0){
                if(best >= 0){ running = best; scen_proc[running].state = RUNNING; scen_proc[running].age = 0; }//best가 음수는 큐가 비어있어서 꺼낼거 없음
            } else if(best >= 0 && best != running
                      && scen_proc[best].cur_priority < scen_proc[running].cur_priority){
                scen_proc[running].state = READY;
                running = best; scen_proc[running].state = RUNNING; scen_proc[running].age = 0;
            }
        }


        if(running >= 0){
            Process* p = &scen_proc[running];
            bool has_event = (p->event_idx >= 0 && p->events != NULL);
            if(has_event && p->executed_cpu == p->events[p->event_idx].start_time){ //인터럽트 발생
                p->state = WAITING; blocked_idx = running; running = -1; is_interrupt = true;
                EVENT* ev = &p->events[p->event_idx];
                ev->left_time--; p->total_blocked_time++; p->IO_burst_time++;
                if(ev->left_time == 0){
                    p->event_idx++; if(p->event_idx == p->event_n) p->event_idx = -1;
                    p->state = READY; is_interrupt = false; blocked_idx = -1;
                }
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -(p->pid + 2);
            } else { //정상작동
                int tq = MAX_TIME; tick_run(running, &tq);
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = running;
            }
        } else { 
            if(is_interrupt){//인터럽트 처리중
                Process* bp = &scen_proc[blocked_idx];
                EVENT* ev = &bp->events[bp->event_idx];
                ev->left_time--; bp->total_blocked_time++; bp->IO_burst_time++;
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -(bp->pid + 2);
                if(ev->left_time == 0){
                    bp->event_idx++; if(bp->event_idx == bp->event_n) bp->event_idx = -1;
                    bp->state = READY; is_interrupt = false; blocked_idx = -1;
                }
            } else {//노는상태
                if(gantt_n < MAX_GANTT) gantt_pid[gantt_n++] = -1;
                continue;
            }
        }
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
