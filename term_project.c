#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>

#define MAX_PROCESS  10     /* 최대 프로세스 수 */
#define MAX_TIME     2000   /* 시뮬레이션 안전 한계 시간 */
#define TIME_QUANTUM 3      /* Round Robin 의 타임 퀀텀 */
#define NUM_PROCESS  5      /* 이번 실행에서 생성할 프로세스 수 */

typedef enum{
    FCFS, SJF_NP, SJF_P, PRIO_NP, PRIO, RR    
} Schedule_Type;

typedef enum{
    NEW, READY, RUNNING, WAITING, TERMINATED
} State;

typedef struct {
    int pid;    int arrival_time;   int cpu_burst;  int priority;
    State state;
    int remaining_cpu;  int executed_cpu;

    int completion_time;    int turnaround_time;    int waiting_time;
} Process;

int     n = 0;                       /* 프로세스 개수 */
Process original[MAX_PROCESS];       /* 원본 (모든 알고리즘이 공유) */
Process proc[MAX_PROCESS];           /* 현재 실행용 작업 복사본 */
 
int ready_q[MAX_PROCESS];  int ready_count = 0;   /* Ready Queue (프로세스 인덱스 저장) */
int wait_q[MAX_PROCESS];   int wait_count  = 0;   /* Waiting Queue (I/O 중) */

void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i){
    original[i].pid          = i + 1;
    original[i].arrival_time = arrival_time;
    original[i].cpu_burst    = cpu_burst;
    original[i].priority     = priority;
}

void create_process(int num) {
    n = num;
    for (int i = 0; i < n; i++) {
        //create_one_process();
        continue;
    }
}

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
