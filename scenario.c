#include <stdio.h>
#include "scenario.h"
#include "globals.h"
#include "utils.h"

void create_one_process(int pid, int arrival_time, int cpu_burst, int priority, int i){
    scen_proc[i].pid          = i + 1;
    scen_proc[i].arrival_time = arrival_time;
    scen_proc[i].cpu_burst    = cpu_burst;
    scen_proc[i].priority     = priority;
    scen_proc[i].state        = NEW;
    return;
}
void create_interrupt(int start_time, int duration, int target_pid, EVENT_Type type, int i){
    scen_intr[i].start_time = start_time;
    scen_intr[i].duration   = duration;
    scen_intr[i].target_pid = target_pid;
    scen_intr[i].type       = type;
    scen_intr[i].left_time  = duration;
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
        /*
        r = rand_range(1,100);
        if((r<=intr_percentage)&&(scen_intr_n<MAX_EVENTS)){
            create_interrupt(ti,rand_range(1,5),0,0,scen_intr_n);
            scen_intr_n++;
        } --- 인터럽트는 일단 무시 ---
        */
        ti++;
    }
}

void process_interrupt_check(void){
    for(int i=0;i<scen_proc_n;i++){
        Process p = scen_proc[i];
        printf("pid : %d, state : %d start_time : %d duration : %d\n",p.pid,p.state,p.arrival_time,p.cpu_burst);
    }
    /*
    printf("\n");
    for(int i=0;i<scen_intr_n;i++){
        EVENT intr = scen_intr[i];
        printf("start : %d, dur : %d, target_pid : %d, type : %d\n",intr.start_time,intr.duration,intr.target_pid,intr.type);
    }
    */ --- 인터럽트는 일단 무시 ---
}