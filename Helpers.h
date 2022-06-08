#include "TCoada.h"
#include "TStiva.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>


typedef struct Thread{
    int id;
}Thread;

typedef struct Task{
    int id;
    struct Thread* thread;
    int prio;
    unsigned int time_left;
    unsigned int time_executed;
}Task;

int AfisTask(void* task, FILE* g); //Afiseaza elem unui task
void add_tasks(void* waitQ, bool* ids, int nr,
 unsigned int time, int prio, FILE* g); // Adauga nr task-uri in waiting queue
void sortQ(void* Queue); // Sorteaza queue-ul
int comp_tasks(void* task1, void* task2); // Compara 2 task-uri
void AfisWaiting(void* waitQ, FILE* g);//Afiseaza waiting queue
void AfisRunning(void* runQ, FILE* g);//Afiseaza running queue
void AfisFinished(void* finishQ, FILE* g);//Afiseaza finished queue
int AfisTaskFinish(void* task, FILE* g);//Afiseaza un task din finished queue
int AfisTaskRun(void* Task, FILE* g);//Afiseaza un task din running queue
int AfisTaskWait(void* task, FILE* g);//Afiseaza un task din waiting queue
void Run(void* waitQ, void* runQ, void* finishQ,
 void* threadS, unsigned int time, bool* ids);//Ruleaza time ms pe 'procesor'
int AfisThread(void* th, FILE* g);//Afiseaza elem unui thread
void getTask(void* waitQ, void* runQ,
 void* finishQ, int task_id, FILE* g);//Cauta task-ul cu id-ul task_id in queues
void getThread(void* runQ, int thread_id, FILE* g); //Cauta thread-ul cu id
int getMinRunTime(void* runQ);//Intoarce cel mai mic timp ramas de rulat
int getMaxRunTime(void* runQ);//Inttoarce cel mai mare timp ramas de rulat
