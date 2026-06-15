#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "scenario.h"
#include "utils.h"
#include "scheduler.h"
#include "getput.h"
#include "pqueue.h"

int main(void) {
    srand(time(NULL));

    bool Was_Scheduled[8]={false,};

    int inpu=1;
    printf("default진행여부\nNo = 0\nYes = 1\n");
    scanf("%i",&inpu);
    //전역구조체 변수접근
    if(inpu==1){
        cfg.cpu_burst_min    = 8;   cfg.cpu_burst_max    = 18;
        cfg.normal_burst_min = 5;   cfg.normal_burst_max = 12;
        cfg.io_burst_min     = 5;   cfg.io_burst_max     = 9;
        cfg.time_quantum     = 3;
        cfg.Max_Process=6; cfg.Max_Time=150;
    }
    else{
        /* 입력받은 [4, max] 를 3등분해 bound별 범위 결정 (최소 4 고정) */
        int burst_max;
        printf("시나리오 최대 시간(최대 2000까지만): ");
        scanf("%d", &cfg.Max_Time);
        if(cfg.Max_Time>2000) cfg.Max_Time=2000;
        printf("시나리오 내 프로세스 최대 개수(최대 100까지만): ");
        scanf("%d",&cfg.Max_Process);
        if(cfg.Max_Process>100) cfg.Max_Process=100;
        printf("CPU 버스트 최대값 (최소는 4 고정): ");
        scanf("%d", &burst_max);
        printf("Round Robin 타임 퀀텀: ");
        scanf("%d", &cfg.time_quantum);
        if(cfg.time_quantum < 1) cfg.time_quantum = 1;
        

        int lo = 4;
        if(burst_max < lo + 6) burst_max = lo + 6;   /* 밴드 뭉개짐 방지 */
        int span = burst_max - lo;
        int a = lo + span/3;                         /* 1/3 지점 */
        int b = lo + (2*span)/3;                      /* 2/3 지점 */
        cfg.io_burst_min     = lo; cfg.io_burst_max     = a;          /* 하위 1/3 : IO_Bound  */
        cfg.normal_burst_min = a;  cfg.normal_burst_max = b;          /* 중간 1/3 : Normal    */
        cfg.cpu_burst_min    = b;  cfg.cpu_burst_max    = burst_max;  /* 상위 1/3 : CPU_Bound */
    }

    printf("making senario...\n");
    create_senario();
    printf("senario making complete\n");

    process_interrupt_check(void);

    while (1)
    {
        printf("\n\n\n which scheduler you want?\n");
        printf("FCFS : 1\n SJF_NP : 2\n SJF_P : 3\n PRIO_NP : 4\n PRIO : 5\n RR : 6\n AGING_PRIO : 7\n MULTIBOUNDQUEUE : 8\n");

        scanf("%d",&inpu);

        switch (inpu)
            {
           case 1:{
                simulate_6(FCFS);
                print_result(FCFS);
                Was_Scheduled[0]=true;
                break;
            }
            case 2:{
                simulate_6(SJF_NP);
                print_result(SJF_NP);
                Was_Scheduled[1]=true;
                break;
            }
            case 3:{
                simulate_6(SJF_P);
                print_result(SJF_P);
                Was_Scheduled[2]=true;
                break;
            }
            case 4:{
                simulate_6(PRIO_NP);
                print_result(PRIO_NP);
                Was_Scheduled[3]=true;
                break;
            }
            case 5:{
                simulate_6(PRIO);
                print_result(PRIO);
                Was_Scheduled[4]=true;
                break;
            }
            case 6:{
                simulate_6(RR);
                print_result(RR);
                Was_Scheduled[5]=true;
                break;
            }
            case 7:{
                simulate_with_aging_priority();
                print_result(AGING);
                Was_Scheduled[6]=true;
                break;
            }
            case 8:{
                simulate_with_multibound();
                print_result(MULTIBOUND);
                Was_Scheduled[7]=true;
                break;
            }
            default:
                break;
        }
        printf("continue?\nNo = 0\nYes = 1\n");
        scanf("%i",&inpu);
        if(inpu!=1) break;
    }
    for(int k=0;k<8;k++){
            if(!Was_Scheduled[k]) continue;
            printf("------------------------------------------------------------------------\n");

            printf("%d\n Completed : %d / %d\n",k ,result_done[k], result_total[k]);
            printf(" Avg Waiting    (completed only) : %.2f\n", result_wait[k]);
            printf(" Avg Turnaround (completed only) : %.2f\n", result_turn[k]);
    }

    
    printf("program end\n");

    return 0;
}