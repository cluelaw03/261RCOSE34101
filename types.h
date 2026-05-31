#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_PROCESS  20     /* 최대 프로세스 수 */
#define MAX_EVENTS     50          /* 사전 생성 인터럽트 시나리오 최대 개수 */
#define MAX_TIME     500   /* 시뮬레이션 안전 한계 시간 */
#define MAX_GANTT    500   /* 간트 차트 최대 길이 */
#define TIME_QUANTUM 3      /* Round Robin 의 타임 퀀텀 */
#define ALG_N         6      /* 알고리즘 수 */
#define CPU_MIN        4
#define CPU_MAX        12
#define PRIO_MIN       1
#define PRIO_MAX       5

/* 타입 정의 -------------------------------------------------------- */
typedef enum{
    FCFS, SJF_NP, SJF_P, PRIO_NP, PRIO, RR   
} Schedule_Type;

typedef enum{
    NEW, READY, RUNNING, WAITING, TERMINATED
} State;

typedef enum{
    CPU_Bound, Normal, IO_Bound
}Process_type;

typedef enum{
    IO, SYSCALL, TIMEOUT
} EVENT_Type;
/* ----------------------------------------------------------------- */

/* 구조체 자료형 정의--------------------------------------------------- */
typedef struct {
    int pid;    int arrival_time;   int cpu_burst;  int priority;
    State state;
    Process_type type;
    int remaining_cpu;  int executed_cpu; int total_blocked_time;

    bool finished;
    int blocked;
    //evaluation용
    int completion_time; int turnaround_time; int waiting_time; int IO_burst_time;
    EVENT* events; int event_n;
} Process;

typedef struct{
    int start_time;  int target_pid; int duration; 
    EVENT_Type type;
} EVENT;
/* ----------------------------------------------------------------- */

#endif /* TYPES_H */