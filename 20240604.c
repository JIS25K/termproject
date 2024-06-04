#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int cpuburst;
    int ioburst;
    int arrival;
    int priority;
} Process;

int waiting_time[6] = {0};
int turnaround_time[6] = {0};

typedef struct {
    Process processes[MAX_PROCESSES];
    int size;
} Queue;

Queue ready_queue;
Queue waiting_queue;

int process_num; 

Process ori[MAX_PROCESSES]; 
Process p[MAX_PROCESSES]; 

int no_print = 0;

void create_process(int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        Process p;
        printf("Enter pid %d: ", i + 1);
        scanf("%d", &p.pid);
        p.cpuburst = rand() % 20 + 1;
        p.ioburst = rand() % 10 + 1;
        p.arrival = rand() % 50;
        p.priority = rand() % 10 + 1;
        ready_queue.processes[ready_queue.size++] = p;
    }
}

void Config() {
    ready_queue.size = 0;
    waiting_queue.size = 0;
}

void schedule(void){
    int sch;

    switch (sch)
    {
    case 1:
        FCFS();
        break;
    case 2:
        SJF();
        break;
    case 3:
        Priority();
        break;
    case 4:
        RR();
        break;
    case 5:
        preemptive_SJF();
        break;
    case 6:
        preemtive_Priority();
        break;
    }
}

void copy_process(void){
    for(int i=0; i<process_num; i++)
        p[i] = ori[i];
}

void print_Gantt(int start, int end, int idx){
    if(no_print) return; 

    printf("%d~%d: ", start, end);

    if(idx == -1) printf("IDLE\n"); 
    else printf("[process %d] used CPU\n", ori[idx].pid); 
}

void FCFS(void){
    int time = 0; 
    int finished = 0; 

    while(finished < process_num){
        int idx = -1;
        
        for(int i=0; i<process_num; i++){
            if(!p[i].cpuburst) continue;
            
            if(idx == -1 || p[i].arrival < p[idx].arrival) 
                idx = i;
        }

        if(p[idx].arrival > time){ 
            print_Gantt(time, p[idx].arrival, -1);
            time = p[idx].arrival;
        }

        print_Gantt(time, time + p[idx].cpuburst, idx);

        waiting_time[0] += (time - p[idx].arrival);
        turnaround_time[0] += (time - p[idx].arrival + p[idx].cpuburst);

        time += p[idx].cpuburst;
        p[idx].cpuburst = 0;
        finished++;
    }
}

void SJF(void){
    int time = 0; 
    int finished = 0; 

    while(finished < process_num){
        int idx = -1;

        for(int i=0; i<process_num; i++){
            if(!p[i].cpuburst) continue;

            if(p[i].arrival <= time) 
                if(idx == -1 || p[i].cpuburst< p[idx].cpuburst || (p[i].cpuburst == p[idx].cpuburst && p[i].arrival < p[idx].arrival))
                    idx = i; 
        }
        
        if(idx == -1){ 
            for(int i=0; i<process_num; i++){ 
                if(!p[i].cpuburst) continue;

                if(idx == -1 || p[i].arrival < p[idx].arrival || (p[i].arrival == p[idx].arrival && p[i].cpuburst < p[idx].cpuburst))
                    idx = i;
            }

            print_Gantt(time, p[idx].arrival, -1); 
            time = p[idx].arrival;
        }

        print_Gantt(time, time + p[idx].cpuburst, idx);

        waiting_time[1] += (time - p[idx].arrival);
        turnaround_time[1] += (time - p[idx].arrival + p[idx].cpuburst);

        time += p[idx].cpuburst;
        p[idx].cpuburst = 0;
        finished++;
    }
}

void Priority(void){
    int time = 0; 
    int finished = 0; 

    while(finished < process_num){
        int idx = -1;

        for(int i=0; i<process_num; i++){
            if(!p[i].cpuburst) continue;

            if(p[i].arrival <= time)
                if(idx == -1 || p[i].priority > p[idx].priority || (p[i].priority == p[idx].priority && p[i].arrival < p[idx].arrival))
                    idx = i; 
        }
        
        if(idx == -1){ 
            for(int i=0; i<process_num; i++){ 
                if(!p[i].cpuburst) continue;

                if(idx == -1 || p[i].arrival < p[idx].arrival || (p[i].arrival == p[idx].arrival && p[i].priority > p[idx].priority))
                    idx = i;
            }

            print_Gantt(time, p[idx].arrival, -1); 
            time = p[idx].arrival;
        }

        print_Gantt(time, time + p[idx].cpuburst, idx);

        waiting_time[2] += (time - p[idx].arrival);
        turnaround_time[2] += (time - p[idx].arrival + p[idx].cpuburst);

        time += p[idx].cpuburst;
        p[idx].cpuburst = 0;
        finished++;
    }
}

void RR(void){
    int quantum = 0;
    int time = 0; 
    int finished = 0; 

    if(no_print == 0){ 
        printf("RR time quantum: ");
        scanf("%d", &quantum);
    }

    while(finished < process_num){
        int idx = -1;
        int end; 
        
        for(int i=0; i<process_num; i++){
            if(!p[i].cpuburst) continue;
            
            if(idx == -1 || p[i].arrival < p[idx].arrival) 
                idx = i;
        }

        if(p[idx].arrival > time){
            print_Gantt(time, p[idx].arrival, -1);
            time = p[idx].arrival;
        }

        end = time;

        waiting_time[3] += (time - p[idx].arrival);

        if(p[idx].cpuburst <= quantum){ 
            turnaround_time[3] += (time - p[idx].arrival + p[idx].cpuburst);

            end += p[idx].cpuburst;
            p[idx].cpuburst = 0;
            finished++;
        }
        else{ 
            turnaround_time[3] += (time - p[idx].arrival + quantum);

            end += quantum;
            p[idx].cpuburst -= quantum;
            p[idx].arrival = end;
        }

        print_Gantt(time, end, idx);

        time = end; 
    }
}

void preemptive_SJF(void){
    int time = 0; 
    int finished = 0; 

    int cur = -1; 
    int cur_started = 0; 

    while(finished < process_num){
        int next = find_shortest(time, cur); 

        if(next != -1){ 
            if(cur == -1){ 
                if(cur_started != time)
                    print_Gantt(cur_started, time, -1);
            }

            else{ 
                print_Gantt(cur_started, time, cur);

                p[cur].arrival = time; 
            }

            waiting_time[4] += (time - p[next].arrival);
            turnaround_time[4] += (time - p[next].arrival);

            cur = next;
            cur_started = time; 
        }

        time++;

        if(cur != -1){ 
            p[cur].cpuburst--;

            turnaround_time[4]++;

            if(p[cur].cpuburst == 0){ 
                print_Gantt(cur_started, time, cur);
                finished++;

                cur = -1;
                cur_started = time;
            }
        }
    }
}

int find_shortest(int time, int cur){
    int idx = -1; 
    
    for(int i=0; i<process_num; i++){
        if(i == cur || p[i].cpuburst == 0 || p[i].arrival > time) continue;

        if(idx != -1){ 
            if(p[i].cpuburst < p[idx].cpuburst || (p[i].cpuburst == p[idx].cpuburst && p[i].arrival < p[idx].arrival))
                idx = i; 
        }

        else if(cur != -1){ 
            if(p[i].cpuburst < p[cur].cpuburst)
                idx = i;
        }

        else{ 
            idx = i;
        }
    }

    return idx;
}


void preemptive_Priority(void){
    int time = 0; 
    int finished = 0; 

    int cur = -1; 
    int cur_started = 0; 


    while(finished < process_num){
        int next = find_highest(time, cur); 

        if(next != -1){ 
            if(cur == -1){ 
                if(cur_started != time)
                    print_Gantt(cur_started, time, -1);
            }

            else{ 
                print_Gantt(cur_started, time, cur);

                p[cur].arrival = time; 
            }

            waiting_time[5] += (time - p[next].arrival);
            turnaround_time[5] += (time - p[next].arrival);

            cur = next;
            cur_started = time; 
        }

        time++;

        if(cur != -1){ 
            p[cur].cpuburst--;

            turnaround_time[5] += 1;

            if(p[cur].cpuburst == 0){ 
                print_Gantt(cur_started, time, cur);
                finished++;

                cur = -1;
                cur_started = time;
            }
        }
    }
}

int find_highest(int time, int cur){
    int idx = -1; 

    for(int i=0; i<process_num; i++){
        if(i == cur || p[i].cpuburst == 0 || p[i].arrival > time) continue;

        if(idx != -1){ 
            if(p[i].priority > p[idx].priority || (p[i].priority == p[idx].priority && p[i].arrival < p[idx].arrival))
                idx = i; 
        }

        else if(cur != -1){ 
            if(p[i].priority > p[cur].priority)
                idx = i;
        }

        else{ 
            idx = i;
        }
    }

    return idx;
}

void Evaluation();
void BestScheduling();