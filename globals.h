#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"

extern Process scen_proc[MAX_PROCESS];
extern Interrupt scen_intr[MAX_EVENTS];
extern Process test_proc[MAX_PROCESS];
extern Interrupt test_intr[MAX_EVENTS];

extern int scen_proc_n;                 /* 시나리오 생성된 프로세스 수 */
extern int scen_intr_n;                 /* 시나리오 생성된 인터럽트 수 */
extern int proc_n;
extern int intr_n;

extern int ready_q[MAX_PROCESS];  extern int ready_count;   /* Ready Queue (프로세스 인덱스 저장) */
extern int wait_q[MAX_PROCESS];   extern int wait_count;    /* Waiting Queue (I/O 중) */
extern int proc_percentage;              /* 시나리오 생성 시 사용할 확률 (%) */
extern int intr_percentage;             /* 시나리오 생성 시 사용할 확률 (%) */


/* 알고리즘별 결과 저장 */
extern double      result_wait[ALG_N];
extern double      result_turn[ALG_N];
extern int         result_done[ALG_N];   /* 완료한 프로세스 수 */
extern int         result_total[ALG_N];  /* 시나리오상 등장한 프로세스 수 (fork 포함) */


extern char *schedule_name[ALG_N];

#endif /* GLOBALS_H */