#include <stdio.h>
#include "scenario.h"
#include "globals.h"
#include "utils.h"

void create_one_process(int arrival_time, int priority, int i){ 
    //프로세스 아이디는 시나리오 생성중에 발생하지 않고, 스케줄링때 생성된다고 가정
    scen_proc[i].arrival_time = arrival_time;
    scen_proc[i].priority     = priority;
    scen_proc[i].state        = NEW;
    int rand = rand_range(1,100);

    //10% CPU_Bound, 30% Normal, 60% IO_Bound
    if(rand<=10){
        scen_proc[i].type         = CPU_Bound;
        scen_proc[i].cpu_burst    = rand_range(20, 50);
    }
    else if(rand<=40){
        scen_proc[i].type         = Normal;
        scen_proc[i].cpu_burst    = rand_range(10, 30);
    }
    else{
        scen_proc[i].type         = IO_Bound;
        scen_proc[i].cpu_burst    = rand_range(5, 15);
    }

    create_IOs(&scen_proc[i]);

    return;
}
EVENT create_TIMEOUT(Process* proc){
    EVENT timeout_event;
    timeout_event.type=TIMEOUT;
    timeout_event.target_pid=proc->pid;
    timeout_event.duration=0;
    timeout_event.left_time=0;
    timeout_event.finished=false;

    return timeout_event;
}


void create_IO(Process* proc){
    if(proc->event_n<=0){
        proc->events=NULL;
        return;
    }
    
    int seg_size = proc->cpu_burst / (proc->event_n + 1);
        if (seg_size < 2) {
        proc->event_n = 0;
        proc->events  = NULL;
        return;
    }

    proc->events = (EVENT *)malloc(sizeof(EVENT) * proc->event_n);
    if (!proc->events) {
        proc->event_n = 0;
        return;
    }
    for (int i = 0; i < proc->event_n; i++){
        int seg_start = seg_size * i;
        int seg_end   = seg_size * (i + 1) - 1;

        if (seg_start < 1)                       seg_start = 1;
        if (seg_end   >= proc->cpu_burst - 1)    seg_end   = proc->cpu_burst - 2;

        int start = (seg_start <= seg_end)
                    ? rand_range(seg_start, seg_end)
                    : (seg_start + seg_end) / 2;

        proc->events[i].start_time = start;
        proc->events[i].duration   = rand_range(1, 5);
        proc->events[i].type       = IO;
    }
}
void create_IOs(Process* proc){
    int n= proc->cpu_burst;
    int rand;
    switch (proc->type){
        case CPU_Bound:
            proc->event_n = rand_range(0, imax(1, n/20));
            break;
        case Normal:
            proc->event_n = rand_range(imax(1, n/15), imax(2, n/8));
            break;
        case IO_Bound:
            proc->event_n = rand_range(imax(1, n/5), imax(2, n/3));
            break;
        default:
            proc->event_n = 0;
            break;
    }

    /* 2. 상한 보정: seg_size >= 2 보장 위해 event_n <= n/2 */
    int max_events = n / 2;
    if (proc->event_n > max_events) proc->event_n = max_events;
    if (proc->event_n < 0)          proc->event_n = 0;

    /* 3. IO_Bound인데 event_n이 0이 되는 사태 방지 */
    if (proc->type == IO_Bound && proc->event_n == 0 && n >= 4)
        proc->event_n = 1;

    /* 4. 이벤트 배치 */
    create_IO(proc);
}
void create_senario(void) {
    int ti=0;
    scen_proc_n=0;
    while(ti<MAX_TIME){
        int r = rand_range(1,100);
        if((r<=proc_percentage)&&(scen_proc_n<MAX_PROCESS)){
            create_one_process(ti,rand_range(1,10),scen_proc_n);
            scen_proc_n++;
        }
        ti++;
    }
}

void process_interrupt_check(void){
    for(int i=0;i<scen_proc_n;i++){
        Process p = scen_proc[i];
        printf("pid : %d, state : %d start_time : %d duration : %d\n",p.pid,p.state,p.arrival_time,p.cpu_burst);
    }
}