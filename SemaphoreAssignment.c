#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

int i;
int j;
sem_t mutex;
int mutValue;
int parent;
int child1;
int child2;
int grandchild;


void* watchMovie()
{
    printf("Waiting-----\n");
    
  
  printf("Value of SEM BEFORE is %d\n", mutValue);
    if(sem_wait(&mutex) == 0 && mutValue == 0)
    {
        int currentPID = getpid();
        if(currentPID == parent)
        {
            printf("Parent Process is waiting until people leave the theatre.........\n");
        }
        else if(currentPID == child1)
        {
            printf("Child 1 Process is waiting until people leave the theatre.........\n");
        }
        else if(currentPID == child2)
        {
            printf("Child 2 Process is waiting until people leave the theatre.........\n");
        }
        else
        {
            printf("Grandchild Process is waiting until people leave the theatre.........\n");
        }
        
    }



    sem_getvalue(&mutex, &mutValue);
    printf("Value of SEM AFTER WAIT is %d\n", mutValue);
    printf("Starting the movie please sit down\n");
    //sleep(1);
      for(int i = 0; i < 3; i++)
    {
        for (int j = 1; j < 100; j++)
        {
            if(j == 90)
            {
                printf("Please start getting Up from your seat Move is over for customer %d\n", i);
            }
        }
    }
    printf("Move is Complete Thank you for watching\n");
    sem_post(&mutex);
}

int main(int argc, char* argv[])
{
    sem_init(&mutex, 1, 2);
    int id1;
    int id2;
  

    id1 = fork();
    id2 = fork();

    if(id1 == 0)
    {
        if(id2 == 0)
        {
            grandchild = getpid();
            printf("Grandchild Process = %d\n", grandchild);
        }
        else{
            child1 = getpid();
        }
    }
    else
    {
        if(id2 == 0)
        {
            child2 = getpid();
        }
        else
        {
            parent = getpid();
        }
        
    }

    watchMovie();

    wait(NULL);

    exit(0);

}