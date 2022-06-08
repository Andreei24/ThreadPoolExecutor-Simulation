#include "Helpers.h"
#define MAX 32767

int AfisTask(void* task, FILE* g){
    Task* aux = (Task*) task;

    fprintf(g,"Id:%d, Prio:%d, Time:%u \n",aux->id, aux->prio, aux->time_left);

    return  1;
}

int AfisTaskWait(void* task, FILE* g){
    Task* aux = (Task*) task;
    fprintf(g,"(%d: priority = %d, remaining_time = %u)",aux->id,aux->prio,aux->time_left);

    return 1;
}

int AfisTaskRun(void* task, FILE* g){
    Task* aux = (Task*) task;
    fprintf(g,"(%d: priority = %d, remaining_time = %u, running_thread = %d)",
    aux->id,aux->prio,aux->time_left, ((Thread*)(aux->thread))->id);

    return 1;
} 

int AfisTaskFinish(void* task, FILE* g){
    Task* aux = (Task*) task;
    fprintf(g,"(%d: priority = %d, executed_time = %u)",aux->id, aux->prio, aux->time_executed);

    return 1;
}

int AfisThread(void* th, FILE* g){
    Thread* aux = (Thread*) th;
    fprintf(g,"Thread: %d \n",aux->id);

    return 1;
}

void AfisWaiting(void* waitQ, FILE* g){
    
    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    
    fprintf(g,"====== Waiting queue =======\n");
    fprintf(g,"[");

    while(!VidaQ(waitQ)){
        ExtrQ(waitQ,aux);
        AfisTaskWait(aux,g);
        if(!VidaQ(waitQ))
            fprintf(g,",\n");
        
        IntrQ(aux_q,aux);
    }

    ConcatQ(waitQ, aux_q);
    DistrQ(&aux_q);
    fprintf(g,"]\n");
}

void AfisRunning(void* runQ, FILE* g){
    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    
    fprintf(g,"====== Running in parallel =======\n");
    fprintf(g,"[");

    while(!VidaQ(runQ)){
        ExtrQ(runQ,aux);
        AfisTaskRun(aux,g);
        if(!VidaQ(runQ))
            fprintf(g,",\n");
        
        IntrQ(aux_q,aux);
    }

    ConcatQ(runQ, aux_q);
    DistrQ(&aux_q);
    fprintf(g,"]\n");
}

void AfisFinished(void* finishQ, FILE* g){
    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    
    fprintf(g,"====== Finished queue =======\n");
    fprintf(g,"[");

    while(!VidaQ(finishQ)){
        ExtrQ(finishQ,aux);
        AfisTaskFinish(aux,g);
        if(!VidaQ(finishQ))
            fprintf(g,",\n");
        
        IntrQ(aux_q,aux);
    }

    ConcatQ(finishQ, aux_q);
    DistrQ(&aux_q);
    fprintf(g,"]\n");
}

int comp_tasks(void* task1, void* task2){
    Task* aux1 = (Task*) task1;
    Task* aux2 = (Task*) task2;
    // DESC PRIO > CRESC TIME > CRESC ID

    if(aux1->prio > aux2->prio)
        return 1;
    if(aux1->prio < aux2->prio)
        return -1;

    if(aux1->time_left < aux2->time_left)
        return 1;
    if(aux1->time_left > aux2->time_left)
        return -1;

    if(aux1->id < aux2->id)
        return 1;
    else
        return -1;
    

}

void sortQ(void* Queue){
    int count = 0;
    void* aux2 = calloc(1,sizeof(Task));

    void* aux_q = InitQ(sizeof(Task),MAX);

    while(!VidaQ(Queue)){
        ExtrQ(Queue,aux2);
        count++;
        IntrQ(aux_q,aux2);
    }
    ConcatQ(Queue,aux_q);
    DistrQ(&aux_q);
    
    void* curr_max = malloc(sizeof(Task));
    void* aux = malloc(sizeof(Task));

    for(int i = 0; i <= count -1; i++){
        ExtrQ(Queue,curr_max);
        for(int j = 1; j <= count -1; j++){
            ExtrQ(Queue,aux);

            if(comp_tasks(curr_max,aux) == 1){
                IntrQ(Queue,curr_max);
                memcpy(curr_max, aux,sizeof(Task));
            }
            else{
                IntrQ(Queue,aux);
            }
        }
        IntrQ(Queue,curr_max);
    }
}

void add_tasks(void* waitQ, bool* ids, int nr, unsigned int time, int prio, FILE* g){
    for(int i = 0; i < nr; i++){
        Task* task = malloc(sizeof(Task));
        
        for(int j = 1; j <= MAX; j++)
            if(ids[j] == false){
                task->id = j;
                ids[j] = true;
                break;
            }
        
        task->prio = prio;
        task->time_left = time;
        task->time_executed = 0;
        task->thread = malloc(sizeof(Thread));
        fprintf(g,"Task created successfully : ID %d.\n",task->id);


        IntrQ(waitQ, task);
    }
    sortQ(waitQ);
}

void Run(void* waitQ, void* runQ, void* finishQ, void* threadS, unsigned int time, bool* ids){

    void* task = malloc(sizeof(Task));
    void *thread = malloc(sizeof(Thread));
    
    while(!VidaS(threadS) && !VidaQ(waitQ)){
        ExtrQ(waitQ,task);
        Pop(threadS, thread);

        memcpy(((Task*)task)->thread,thread,sizeof(Thread));

        IntrQ(runQ,task);
    }

    void* aux_q = InitQ(sizeof(Task),128);
    
    while(!VidaQ(runQ)){
        ExtrQ(runQ,task);

        if(((Task*)task)->time_left <= time){
            ((Task*)task)->time_executed += ((Task*)task)->time_left;
            ((Task*)task)->time_left = 0;
            ids[((Task*)task)->id] = false;
            Push(threadS,((Task*)task)->thread);
            IntrQ(finishQ,task);
            }
        else{
            ((Task*)task)->time_executed += time;
            ((Task*)task)->time_left -= time;
            IntrQ(aux_q,task);
        }
    }

    ConcatQ(runQ,aux_q);
    DistrQ(&aux_q);

    while(!VidaS(threadS) && !VidaQ(waitQ)){
        ExtrQ(waitQ,task);
        Pop(threadS, thread);

        memcpy(((Task*)task)->thread,thread,sizeof(Thread));

        IntrQ(runQ,task);
    }
}

void getTask(void* waitQ, void* runQ, void* finishQ, int task_id, FILE* g){

    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    int found = 0;

    while(!VidaQ(runQ)){
        ExtrQ(runQ,aux);
        if(((Task*)aux)->id == task_id){
            fprintf(g,"Task %d is running (remaining_time = %u).\n",((Task*)aux)->id, ((Task*)aux)->time_left);
            found = 1;
            }
        IntrQ(aux_q,aux);
    }

    ConcatQ(runQ,aux_q);
    
    if(found == 1){
        return;
    }

    while(!VidaQ(waitQ)){
        ExtrQ(waitQ,aux);
        if(((Task*)aux)->id == task_id){
            fprintf(g,"Task %d is waiting (remaining_time = %u).\n",((Task*)aux)->id, ((Task*)aux)->time_left);
            found = 1;
            }
        IntrQ(aux_q,aux);
    }

    ConcatQ(waitQ,aux_q);

    if(found == 1)
        return;

    while(!VidaQ(finishQ)){
        ExtrQ(finishQ,aux);
        if(((Task*)aux)->id == task_id){
            if(found == 0)
                fprintf(g,"Task %d is finished (executed_time = %u).\n",((Task*)aux)->id, ((Task*)aux)->time_executed);
            
            found = 1;
            }
        IntrQ(aux_q,aux);
    }

        ConcatQ(finishQ,aux_q);


    if(found == 0){
        fprintf(g,"Task %d not found.\n",task_id);
        return;
    }
}

void getThread(void* runQ, int thread_id, FILE* g){
    
    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    int found = 0;

    while(!VidaQ(runQ)){
        ExtrQ(runQ,aux);
        
        if(((Thread*)(((Task*)aux)->thread))->id == thread_id){
            fprintf(g,"Thread %d is running task %d (remaining_time = %u).\n",
            thread_id,((Task*)aux)->id,((Task*)aux)->time_left);
            found =1;
        }
        IntrQ(aux_q,aux);
    }

    ConcatQ(runQ,aux_q);

    if(found == 0)
        fprintf(g,"Thread %d is idle.\n",thread_id);
}

int getMinRunTime(void* runQ){

    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    int time = -2;

    while(!VidaQ(runQ)){
        ExtrQ(runQ,aux);
        
        if(((Task*)aux)->time_left < time || time == -2){
           time = ((Task*)aux)->time_left;
        }
        IntrQ(aux_q,aux);
    }

    ConcatQ(runQ,aux_q);

    return time;
}

int getMaxRunTime(void* runQ){

    void* aux_q = InitQ(sizeof(Task),MAX);
    void* aux = malloc(sizeof(Task));
    int time = -2;

    while(!VidaQ(runQ)){
        ExtrQ(runQ,aux);
        
        if(((Task*)aux)->time_left > time || time == -2){
           time = ((Task*)aux)->time_left;
        }
        IntrQ(aux_q,aux);
    }

    ConcatQ(runQ,aux_q);

    return time;
}