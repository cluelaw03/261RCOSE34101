#include "globals.h"

Process scen_proc[MAX_PROCESS];

int scen_proc_n=0;                 /* 시나리오 생성된 프로세스 수 */

 
PQueue ready_q;                              /* Ready Queue (우선순위 큐) */
Queue ready_fifo;                              /* Ready Queue (FCFS) */
bool is_ready_fifo;                            /* RR 알고리즘에서 ready_fifo 사용 여부 */
Queue wait_q;                               /* Waiting Queue (I/O 중) */
PQueue JOB_q;                                /* Job Queue (도착 안 한 프로세스) */  

//멀티바운드용
Queue rq_io;
Queue rq_cpu;


int proc_percentage=10;              /* 시나리오 생성 시 사용할 확률 (%) */
int intr_percentage=5;             /* 시나리오 생성 시 사용할 확률 (%) */
bool isblocked;

/* 알고리즘별 결과 저장 */
double      result_wait[ALG_N];
double      result_turn[ALG_N];
int         result_done[ALG_N];   /* 완료한 프로세스 수 */
int         result_total[ALG_N];  /* 시나리오상 등장한 프로세스 수  */

int         gantt_pid[MAX_GANTT];
int         gantt_n;

char *schedule_name[ALG_N] = {
    "FCFS", "Non-Preemptive SJF", "Preemptive SJF",
    "Non-Preemptive Priority", "Preemptive Priority", "Round Robin",
    "Priority + Aging", "Multilevel (IO:FCFS / CPU:RR)"
};

SimConfig cfg = {//기본값 설정
    .cpu_burst_min    = 8,  .cpu_burst_max    = 18,
    .normal_burst_min = 5,  .normal_burst_max = 12,
    .io_burst_min     = 5,  .io_burst_max     = 9,
    .time_quantum     = TIME_QUANTUM
};
