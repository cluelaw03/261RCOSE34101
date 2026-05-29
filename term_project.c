#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PROCESS  20     /* 최대 프로세스 수 */
#define MAX_EVENTS     50          /* 사전 생성 인터럽트 시나리오 최대 개수 */
#define MAX_TIME     500   /* 시뮬레이션 안전 한계 시간 */
#define TIME_QUANTUM 3      /* Round Robin 의 타임 퀀텀 */
#define ALG_N         6      /* 알고리즘 수 */

/* 타입 정의 -------------------------------------------------------- */
typedef enum{
    FCFS, SJF_NP, SJF_P, PRIO_NP, PRIO, RR   
} Schedule_Type;

typedef enum{
    NEW, READY, RUNNING, WAITING, TERMINATED
} State;

typedef enum{
    IO_start,IO_end, SYSCALL_start, SYSCALL_end
} Interrupt_Type;
/* ----------------------------------------------------------------- */

/* 구조체 자료형 정의--------------------------------------------------- */
typedef struct {
    int pid;    int arrival_time;   int cpu_burst;  int priority;
    State state;
    int remaining_cpu;  int executed_cpu;

    bool finished;
    //evaluation용
    int completion_time; int turnaround_time; int waiting_time;
    int total_blocked_time;
} Process;

typedef struct{
    int start_time; int duration;  int target_pid;
    Interrupt_Type type;
} Interrupt;
/* ----------------------------------------------------------------- */

Process scen_proc[MAX_PROCESS];
Interrupt scen_intr[MAX_EVENTS];
Process test_proc[MAX_PROCESS];
Interrupt test_intr[MAX_EVENTS];

int scen_proc_n=0;                 /* 시나리오 생성된 프로세스 수 */
int scen_intr_n=0;                 /* 시나리오 생성된 인터럽트 수 */
int proc_n;
int intr_n;
 
int ready_q[MAX_PROCESS];  int ready_count = 0;   /* Ready Queue (프로세스 인덱스 저장) */
int wait_q[MAX_PROCESS];   int wait_count  = 0;   /* Waiting Queue (I/O 중) */
int proc_percentage=10;              /* 시나리오 생성 시 사용할 확률 (%) */
int intr_percentage=5;             /* 시나리오 생성 시 사용할 확률 (%) */


/* 알고리즘별 결과 저장 */
double      result_wait[ALG_N];
double      result_turn[ALG_N];
int         result_done[ALG_N];   /* 완료한 프로세스 수 */
int         result_total[ALG_N];  /* 시나리오상 등장한 프로세스 수 (fork 포함) */


char *schedule_name[ALG_N] = {
    "FCFS", "Non-Preemptive SJF", "Preemptive SJF",
    "Non-Preemptive Priority", "Preemptive Priority", "Round Robin"
};

void create_senario(void);
void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i);
void create_interrupt(int start_time, int duration, int target_pid, Interrupt_Type type, int i);
void enqueue_ready(int idx);
void remove_ready_at(int pos);
void enqueue_wait(int idx);
void remove_wait_at(int pos);
void process_interrupt_check(void);
int rand_range(int lo, int hi);
void reset_test(void);
void schedule_fcfs(void);

int main(void) {
    srand(time(NULL));
    create_senario();

    
    return 0;
}

void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i){
    scen_proc[i].pid          = i + 1;
    scen_proc[i].arrival_time = arrival_time;
    scen_proc[i].cpu_burst    = cpu_burst;
    scen_proc[i].priority     = priority;
    scen_proc[i].state        = NEW;
    return;
}
void create_interrupt(int start_time, int duration, int target_pid, Interrupt_Type type, int i){
    scen_intr[i].start_time = start_time;
    scen_intr[i].duration   = duration;
    scen_intr[i].target_pid = target_pid;
    scen_intr[i].type       = type;
    return;
}
void create_senario(void) {
    int ti=0;
    scen_proc_n=0; scen_intr_n=0;
    while(ti<MAX_TIME){
        int r = rand_range(1,100);
        if((r<=proc_percentage)&&(scen_proc_n<MAX_PROCESS)){
            create_one_process(scen_proc_n,ti,rand_range(1,10),rand_range(1,5),scen_proc_n);
            scen_proc_n++;
        }
        r = rand_range(1,100);
        if((r<=intr_percentage)&&(scen_intr_n<MAX_EVENTS)){
            create_interrupt(ti,rand_range(1,5),0,(rand_range(0,1) ? IO_start : SYSCALL_start),scen_intr_n);
            scen_intr_n++;
        }
        ti++;
    }
}

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
        test_proc[i].finished          = false;
        test_proc[i].completion_time    = 0;
        test_proc[i].turnaround_time    = 0;
        test_proc[i].waiting_time       = 0;
        test_proc[i].total_blocked_time = 0;
    }
    for (int i = 0; i < intr_n; i++) {
        test_intr[i] = scen_intr[i];
    }
}

/* ---------------- 큐 보조 함수 ---------------- */
void enqueue_ready(int idx) {
    ready_q[ready_count++] = idx;
    test_proc[idx].state = READY;
}
void remove_ready_at(int pos) {                 /* 위치 pos 제거 후 앞으로 당김(FIFO 순서 유지) */
    for (int i = pos; i < ready_count - 1; i++)
        ready_q[i] = ready_q[i + 1];
    ready_count--;
}
void enqueue_wait(int idx) {
    wait_q[wait_count++] = idx;
    test_proc[idx].state = WAITING;
}
void remove_wait_at(int pos) {
    for (int i = pos; i < wait_count - 1; i++)
        wait_q[i] = wait_q[i + 1];
    wait_count--;
}

void process_interrupt_check(void){
    for(int i=0;i<scen_proc_n;i++){
        Process p = scen_proc[i];
        printf("pid : %d, state : %d start_time : %d duration : %d\n",p.pid,p.state,p.arrival_time,p.cpu_burst);
    }
    printf("\n");
    for(int i=0;i<scen_intr_n;i++){
        Interrupt intr = scen_intr[i];
        printf("start_time : %d, duration : %d, target_pid : %d, type : %d\n",intr.start_time,intr.duration,intr.target_pid,intr.type);
    }
}

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