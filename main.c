#include "Helpers.h"
#define MAX 32767


int main(int argc, char* argv[]){

    FILE* f = fopen(argv[1],"r"); //Fisierul de in
    FILE* g = fopen(argv[2],"w+"); // Fisierul de out

    char* input = malloc(250 * sizeof(char));
    char* ptr;
    int N;
    unsigned int Q;
    // array de id-uri, true = ocupat, false = liber
    bool* task_ids = calloc(MAX,sizeof(bool));  
    unsigned int time_passed = 0;
    task_ids[0] = true; // nu este folosit

    fgets(input,250,f);
    ptr = strtok(input," \n");
    Q = atoi(ptr); // cuanta de timp

    fgets(input,250,f);
    ptr = strtok(input," \n");
    N = atoi(ptr)  * 2; // numarul de thread-uri


    void* waitQ = InitQ(sizeof(Task),MAX);
    void* runQ = InitQ(sizeof(Task),N);
    void* finishQ = InitQ(sizeof(Task),MAX);
    void* threadS = InitS(sizeof(Thread),N);

    for(int i = N-1; i >= 0; i--){ // Generarea thread-urilor
        Thread* th = malloc(sizeof(Thread));
        th->id = i;
        Push(threadS,th);
    }

    while(fgets(input,250,f)){
        ptr = strtok(input," \n");

        if(strcmp(ptr,"add_tasks") == 0){//daca comanda este add_tasks

            ptr = strtok(NULL," \n");
            int nr_tasks = atoi(ptr); // numarul de task-uri

            ptr = strtok(NULL," \n");
            unsigned int exec_time = atoi(ptr); // timpul de executie

            ptr = strtok(NULL," \n");
            int prio = atoi(ptr); // prioritatea

            add_tasks(waitQ,task_ids,nr_tasks,exec_time,prio,g);
        }

        if(strcmp(ptr,"print") == 0){ // daca este o comanda de tip print
            ptr = strtok(NULL," \n");

            if(strcmp(ptr,"waiting") == 0){
                AfisWaiting(waitQ,g);
                continue;
            }

            if(strcmp(ptr,"finished") == 0){
                AfisFinished(finishQ,g);
                continue;
            }

            if(strcmp(ptr,"running") == 0){
                AfisRunning(runQ,g);
            }
        }

        if(strcmp(ptr,"run") == 0){

            ptr = strtok(NULL," \n");
            unsigned int run_time = atoi(ptr);
            fprintf(g,"Running tasks for %u ms...\n",run_time);

            if(run_time > Q){ //daca timpul de rulat este mai mare decat o cuanta
                int stop_count = 0;
                
                //Atunci timpul total este spart in cuante care urmeaza sa fie rulate
                for(int i = 0; i < run_time / Q; i++){
                    
                    if(!VidaQ(waitQ) || !VidaQ(runQ)){
                        
                        //Dummy run pentru a determina timpul necesar executiei
                        //pentru comanda finish
                        Run(waitQ,runQ,finishQ,threadS,0,task_ids);

                        if(VidaQ(waitQ) && stop_count == 0){
                        time_passed += getMaxRunTime(runQ);
                        stop_count = 1;
                        }
                        
                        Run(waitQ,runQ,finishQ,threadS,Q,task_ids);
                        
                        
                    }
                }
                
                if(run_time % Q != 0){ // Restul timpului de rulat in cazul
                    //in care run_time nu se imparte exact la Q
                    if(!VidaQ(waitQ) || !VidaQ(runQ)){
                        
                        Run(waitQ,runQ,finishQ,threadS,run_time % Q,task_ids);
                        time_passed += run_time % Q;
                    } 
                }
            }
            else{//Daca este mai mic decat o cuanta, se ruleaza asa
                if(!VidaQ(waitQ) || !VidaQ(runQ)){
                    Run(waitQ,runQ,finishQ,threadS,run_time,task_ids);
                    time_passed += run_time;
                } 
            }
        }

        if(strcmp(ptr,"get_task") == 0){//Cauta task-ul in queues
            ptr = strtok(NULL," \n");
            int task_id = atoi(ptr);

            getTask(waitQ,runQ,finishQ,task_id,g);
        }

        if(strcmp(ptr,"get_thread") == 0){//Cauta thread-ul in stack sau running queue
            ptr = strtok(NULL," \n");
            int thread_id = atoi(ptr);

            if(thread_id < N) // Verifica sa fie un id valid
                getThread(runQ,thread_id,g);
        }

        if(strcmp(ptr,"finish") == 0){
            
            while(!VidaQ(waitQ) || !VidaQ(runQ)){ // ruleaza cat timp exista task-uri
                int minTime = 0;
                minTime = getMinRunTime(runQ);//in cuante de min run time

                Run(waitQ,runQ,finishQ,threadS,minTime,task_ids);
                time_passed += minTime;
            }

            fprintf(g,"Total time: %d",time_passed);
        }
    }


    return 0;
}