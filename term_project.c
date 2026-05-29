#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PROCESS  10     /* 최대 프로세스 수 */
#define MAX_TIME     2000   /* 시뮬레이션 안전 한계 시간 */
#define TIME_QUANTUM 3      /* Round Robin 의 타임 퀀텀 */
#define NUM_PROCESS  5      /* 이번 실행에서 생성할 프로세스 수 */
#define MAX_TIME       120

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

    int completion_time;    int turnaround_time;    int waiting_time;
    bool fininshed;
    //evaluation용
    int completion_time; int turnaround_time; int waiting_time;
    int total_blocked_time;
} Process;

typedef struct{
    int start_time; int duration;  int target_pid;
    Interrupt_Type type;
} Interrupt;
/* ----------------------------------------------------------------- */


Process original[MAX_PROCESS];       /* 원본 (모든 알고리즘이 공유) */
Process proc[MAX_PROCESS];           /* 현재 실행용 작업 복사본 */
Interrupt scen_intr[MAX_PROCESS];   /* 시나리오 생성된 인터럽트들 */
 
int ready_q[MAX_PROCESS];  int ready_count = 0;   /* Ready Queue (프로세스 인덱스 저장) */
int wait_q[MAX_PROCESS];   int wait_count  = 0;   /* Waiting Queue (I/O 중) */
int proc_percentage;              /* 시나리오 생성 시 사용할 확률 (%) */
int intr_percentage;             /* 시나리오 생성 시 사용할 확률 (%) */

int proc_n=0;                 /* 시나리오 생성된 프로세스 수 */
int intr_n=0;                 /* 시나리오 생성된 인터럽트 수 */

void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i){
    original[i].pid          = i + 1;
    original[i].arrival_time = arrival_time;
    original[i].cpu_burst    = cpu_burst;
    original[i].priority     = priority;
    original[i].state        = NEW;
    return;
}

void create_interrupt(int start_time, int duration, int target_pid, Interrupt_Type type, int i){
    Interrupt intr;
    intr.start_time = start_time;
    intr.duration = duration;
    intr.target_pid = target_pid;
    intr.type = type;
    scen_intr[i] = intr;
    return;
}

void create_senario(void) {
    int ti=0;
    proc_n=0; intr_n=0;
    while(ti<MAX_TIME){
        int r = rand_range(1,100);
        if(r<=proc_percentage){
            create_one_process(proc_n,ti,rand_range(1,10),rand_range(1,5),proc_n);
            proc_n++;
        }
        r = rand_range(1,100);
        if(r<=intr_percentage){
            create_interrupt(ti,rand_range(1,5),rand_range(0,proc_n-1),rand_range(0,1) ? IO_start : SYSCALL_start,intr_n);
            intr_n++;
        }
    }
}

/* ---------------- 유틸 ---------------- */
static int rand_range(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

/* ---------------- 큐 보조 함수 ---------------- */
void enqueue_ready(int idx) {
    ready_q[ready_count++] = idx;
    proc[idx].state = READY;
}
void remove_ready_at(int pos) {                 /* 위치 pos 제거 후 앞으로 당김(FIFO 순서 유지) */
    for (int i = pos; i < ready_count - 1; i++)
        ready_q[i] = ready_q[i + 1];
    ready_count--;
}
void enqueue_wait(int idx) {
    wait_q[wait_count++] = idx;
    proc[idx].state = WAITING;
}
void remove_wait_at(int pos) {
    for (int i = pos; i < wait_count - 1; i++)
        wait_q[i] = wait_q[i + 1];
    wait_count--;
}

int main(int argc, char* argv[]) {
    
    return 0;
}
