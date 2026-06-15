#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"
#include "pqueue.h"

extern Process scen_proc[MAX_PROCESS];
extern bool is_duplicated[1000];

extern int scen_proc_n;                 /* 시나리오 생성된 프로세스 수 */

extern PQueue ready_q;                              /* Ready Queue (우선순위 큐) */
extern Queue wait_q;                               /* Waiting Queue (I/O 중) */
extern Queue ready_fifo;                              /* Ready Queue (FCFS) */

/*Multibound 스케줄링 용 큐 2개*/
extern Queue rq_io;           //IO바운드
extern Queue rq_cpu;          //Cpu바운드

extern bool is_ready_fifo;                            /* RR 알고리즘에서 ready_fifo 사용 여부 */
extern PQueue JOB_q;                                /* Job Queue (도착 안 한 프로세스) */
extern int proc_percentage;              /* 시나리오 생성 시 사용할 확률 (%) */
extern int intr_percentage;             /* 시나리오 생성 시 사용할 확률 (%) */
extern bool isblocked;

/* 알고리즘별 결과 저장 */
extern double      result_wait[ALG_N];
extern double      result_turn[ALG_N];
extern int         result_done[ALG_N];   /* 완료한 프로세스 수 */
extern int         result_total[ALG_N];  /* 시나리오상 등장한 프로세스 수 (fork 포함) */


extern int         gantt_pid[MAX_GANTT];
extern int         gantt_n;

extern char *schedule_name[ALG_N];

extern SimConfig cfg;

#endif /* GLOBALS_H */
