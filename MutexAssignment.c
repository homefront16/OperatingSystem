#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

int i;
int bankDollars = 10000000; // Shared Resource 1
int userDollars; // Shared Resource 2
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//sem_t mutex;
//int mutValue;


void* bankTransaction()
{
    printf("Waiting-----\n");
    
  
  
    //sem_wait(&mutex);
    //sem_getvalue(&mutex, &mutValue);

    pthread_mutex_lock(&mutex);
    printf("Making Deposit From Process %d\n", getpid());

      for(int i = 0; i < 1000000; i++)
    {
        bankDollars--;
        userDollars++;
    }
    printf("Deposit Complete From %d\n", getpid());
    pthread_mutex_unlock(&mutex);
    //sem_post(&mutex);
}

int main(int argc, char* argv[])
{

pthread_t thread_1, thread_2;

pthread_create(&thread_1, NULL, &bankTransaction, NULL);
pthread_create(&thread_2, NULL, &bankTransaction, NULL);


pthread_join(thread_1, NULL);
pthread_join(thread_2, NULL);
printf("Threads Joined\n");
printf("Bank Dollars: $ %d\n", bankDollars);
printf("User Dollars: $ %d\n", userDollars);

exit(0);

}