#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

int i;
int filesTransfered = 0; // Shared Resource 1
int fileTransferFailAttempt = 0; // Shared Resource 2
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
double time_spent;
_Bool timesUp = 0;
pthread_t thread_1, thread_2;
clock_t start_t;

void* fileTransfer()
{
    printf("Waiting-----\n");
    
  
  
    //sem_wait(&mutex);
    //sem_getvalue(&mutex, &mutValue);
   
    while(timesUp == 0)
    {
        pthread_mutex_lock(&mutex);
       // pthread_mutex_lock(&mutex2);
        printf("Transferring Files in parent \n");
      //  sleep(1);
        for(int i = 0; i < 10000000; i++)
        {
            filesTransfered++;
        }
        printf("Files Transfered  %d\n", filesTransfered);
        pthread_mutex_unlock(&mutex);
       // pthread_mutex_unlock(&mutex2);
        //sem_post(&mutex);
    }
    
}

void *timerThread(void *a)
{

    while(timesUp == 0)
    {
        time_spent = (double)(clock() - start_t) / CLOCKS_PER_SEC;
 
        if(time_spent > 3)
        {
            timesUp = 1;
        }

        if(pthread_mutex_trylock(&mutex) == 0)
        {
            sleep(1);
            printf("Time Spent = %f\n", time_spent);
            printf("Transferring Files in child \n");
            for(int i = 0; i < 10000000; i++)
            {
                filesTransfered++;
            }
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            fileTransferFailAttempt++;
        }
    }
    
        // pthread_mutex_lock(&mutex2);
        // pthread_mutex_lock(&mutex);
        // //sleep(1);
        // printf("Time Spent = %f\n", time_spent);
        // printf("Transferring Files in child \n");
        // for(int i = 0; i < 1000000; i++)
        // {
        //     filesTransfered++;
        // }
        // pthread_mutex_unlock(&mutex);
        // pthread_mutex_unlock(&mutex2);

        
    if(timesUp == 1 && fileTransferFailAttempt > 0)
    {
        printf("\n In Timer");
        sleep(10);
        printf("\n Killing thread 2\n");
       // pthread_cleanup_push(method, 3);
        pthread_cancel(thread_2);
        
       // pthread_cleanup_pop(1);
        
    }
}

int main(int argc, char* argv[])
{

    start_t = clock();
    time_spent = (double)(clock() - start_t) / CLOCKS_PER_SEC;
   

    pthread_create(&thread_1, NULL, &fileTransfer, NULL);
    pthread_create(&thread_2, NULL, &timerThread, NULL);




    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    printf("Threads Joined\n");
    printf("Total Failed Attempts %d\n", fileTransferFailAttempt);
    sleep(1);
    printf("=======================================================\n");
    printf("total files transferred at end of program:  %d\n", filesTransfered);
    exit(0);

}