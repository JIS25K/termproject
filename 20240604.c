#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_PROCESSES 30


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

char *algo[7] = {"FCFS", "SJF", "Priority", "RR", "preemptive SJF", "preemptive Priority", "Exit"};

int process_num; 

Process original[MAX_PROCESSES]; 
Process p[MAX_PROCESSES]; 

int no_print = 0;

void create_process(int process_num) {

    printf("프로세스의 수를 입력하시오(1~%d): ", MAX_PROCESSES);
    scanf("%d", &process_num);

    for (int i = 0; i <process_num; i++) {
        Process p;
        printf("pid를 입력하시오 %d: ", i + 1);
        scanf("%d", &p.pid);
        p.cpuburst = rand() % 20 + 1;
        p.ioburst = rand() % 10 + 1;
        p.arrival = rand() % 50;
        p.priority = rand() % 10 + 1;
    }
}

void schedule(void){
    int sch;
    while(1){
        printf("사용할 스케쥴링 기법을 선택해주세요, 1:FCFS, 2:SJF, 3:Priority, 4:RR, 5:preemtive_SJF, 6:preemtive_Priority");
        scanf("%d", &sch);

        if(sch == 7) break;

        copy_process();
    }
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
        p[i] = original[i];
}

void print_Gantt(int start, int end, int idx){
    if(no_print) return; 

    printf("%d~%d: ", start, end);

    if(idx == -1) printf("IDLE\n"); 
    else printf("[process %d] used CPU\n", original[idx].pid); 
}

int select_next_process_FCFS(int time) {
    int idx = -1;
    for(int i = 0; i < process_num; i++) {
        if(p[i].cpuburst <= 0) continue; 
        if(idx == -1 || p[i].arrival < p[idx].arrival) idx = i; 
    }
    return idx;
}

void FCFS(void) {
    int time = 0; 
    int finished = 0; 

    while(finished < process_num) {
        int idx = select_next_process_FCFS(time);

        if(p[idx].arrival > time) { 
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


int select_next_process_SJF(int time) {
    int idx = -1;

    for(int i = 0; i < process_num; i++) {
        if(p[i].cpuburst <= 0) continue;

        if(p[i].arrival <= time) {
            if(idx == -1 || p[i].cpuburst < p[idx].cpuburst || (p[i].cpuburst == p[idx].cpuburst && p[i].arrival < p[idx].arrival)) {
                idx = i;
            }
        }
    }

    if(idx == -1) {
        for(int i = 0; i < process_num; i++) {
            if(p[i].cpuburst <= 0) continue;

            if(idx == -1 || p[i].arrival < p[idx].arrival || (p[i].arrival == p[idx].arrival && p[i].cpuburst < p[idx].cpuburst)) {
                idx = i;
            }
        }
    }

    return idx;
}

void SJF(void) {
    int time = 0; 
    int finished = 0; 

    while(finished < process_num) {
        int idx = select_next_process_SJF(time);

        if(p[idx].arrival > time) { 
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

int select_next_process_Priority(int time) {
    int idx = -1;

    for(int i = 0; i < process_num; i++) {
        if(p[i].cpuburst <= 0) continue;

        if(p[i].arrival <= time) {
            if(idx == -1 || p[i].priority > p[idx].priority || (p[i].priority == p[idx].priority && p[i].arrival < p[idx].arrival)) {
                idx = i;
            }
        }
    }

    if(idx == -1) {
        for(int i = 0; i < process_num; i++) {
            if(p[i].cpuburst <= 0) continue;

            if(idx == -1 || p[i].arrival < p[idx].arrival || (p[i].arrival == p[idx].arrival && p[i].priority > p[idx].priority)) {
                idx = i;
            }
        }
    }

    return idx;
}

void Priority(void) {
    int time = 0; 
    int finished = 0; 

    while(finished < process_num) {
        int idx = select_next_process_Priority(time);

        if(p[idx].arrival > time) { 
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

int select_next_process_RR(int time, int process_num, Process p[]) {
    int idx = -1;
    
    for(int i = 0; i < process_num; i++) {
        if(p[i].cpuburst <= 0) continue; 
        
        if(idx == -1 || p[i].arrival < p[idx].arrival) 
            idx = i;
    }
    
    return idx;
}

void RR(void) {
    int quantum = 0;
    int time = 0; 
    int finished = 0; 

    if(no_print == 0) { 
        printf("RR time quantum: ");
        scanf("%d", &quantum);
    }

    while(finished < process_num) {
        int idx = select_next_process_RR(time, process_num, p);
        int end; 
        
        if(p[idx].arrival > time) {
            print_Gantt(time, p[idx].arrival, -1);
            time = p[idx].arrival;
        }

        waiting_time[3] += (time - p[idx].arrival);

        if(p[idx].cpuburst <= quantum) {
            end = time + p[idx].cpuburst;
            turnaround_time[3] += end - p[idx].arrival;
            p[idx].cpuburst = 0;
            finished++;
        } else {
            end = time + quantum;
            p[idx].cpuburst -= quantum;
            p[idx].arrival = end; 
            turnaround_time[3] += end - p[idx].arrival;
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

void Evaluation() {
    int min_wait_idx = -1, min_turn_idx = -1;
    int FLT_MAX;
    float min_avg_wait = FLT_MAX, min_avg_turn = FLT_MAX; 

    for(int i = 0; i < 6; i++) {
        if(turnaround_time[i] == 0) continue; 

        float avg_wait = (float)waiting_time[i] / process_num;
        float avg_turn = (float)turnaround_time[i] / process_num;

        if(avg_wait < min_avg_wait) {
            min_avg_wait = avg_wait;
            min_wait_idx = i;
        }
        if(avg_turn < min_avg_turn) {
            min_avg_turn = avg_turn;
            min_turn_idx = i;
        }
    }

    if(min_wait_idx != -1) { 
        printf("waiting time이 가장 작은 알고리즘은 %s\n", algo[min_wait_idx]);
        printf("turnaround time이 가장 작은 알고리즘은 %s\n", algo[min_turn_idx]);
    }
}


int main() {
    create_proces();
    schedule();
    Evaluation();
}
