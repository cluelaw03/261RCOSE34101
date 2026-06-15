#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define MAX_PROCESS  4     /* 최대 프로세스 수 */
#define MAX_TIME     200   /* 시뮬레이션 안전 한계 시간 */
#define MAX_GANTT    200   /* 간트 차트 최대 길이 */
#define TIME_QUANTUM 3      /* Round Robin 의 타임 퀀텀 */
#define ALG_N         8      /* 알고리즘 수 */
#define PRIO_MIN       1
#define AGING_INTERVAL 5

/* 타입 정의 -------------------------------------------------------- */
typedef enum{
    FCFS, SJF_NP, SJF_P, PRIO_NP, PRIO, RR, AGING, MULTIBOUND
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

typedef struct{
    int start_time;  int target_pid; int duration; 
    int left_time; bool finished;
    EVENT_Type type;
} EVENT;

/* 구조체 자료형 정의--------------------------------------------------- */
typedef struct {
    int real_pid;
    int pid;    int arrival_time;   int cpu_burst;  int priority;
    State state;
    Process_type type;
    int remaining_cpu;  int executed_cpu; int total_blocked_time;

     int cur_priority; int age; //aging에만 사용

    bool finished;
    int blocked;
    //evaluation용
    int completion_time; int turnaround_time; int waiting_time; int IO_burst_time;
    EVENT* events; int event_n; int event_idx;
} Process;
/* ----------------------------------------------------------------- */

//파라미터 조정
typedef struct {
    int cpu_burst_min,    cpu_burst_max;     /* CPU_Bound 버스트 범위 */
    int normal_burst_min, normal_burst_max;  /* Normal   버스트 범위 */
    int io_burst_min,     io_burst_max;      /* IO_Bound 버스트 범위 */
    int time_quantum;                        /* Round Robin 타임 퀀텀 */
} SimConfig;

#endif /* TYPES_H */