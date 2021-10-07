/*
* Author:	Michael Landreth
* Date:		09/30/2021
* Application to demonstrate how fork works.
*/

// The header above is required per your CSET Industry Code Practices.
// This file may violates the code practice's 125 character line length.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include  <signal.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/wait.h>

// Shared Circular Buffer
struct CIRCULAR_BUFFER
{
    int count;          // Number of items in the buffer
    int lower;          // Next slot to read in the buffer
    int upper;          // Next slot to write in the buffer
    int buffer[100];
};
struct CIRCULAR_BUFFER *buffer = NULL;
int i;

void producer (void)
{
    printf("Inside the Producer Method");
    for(i = 0; i < 100; i++)
    {
        buffer->buffer[i] = i + 1;
        buffer->count++;
        buffer->upper = i;
    }

    for(i = 0; i < 150; i++)
    {
        buffer->buffer[i] = i + 1;
        buffer->count++;
        buffer->upper = i;
    }
    _exit(0);
}

void consumer (void)
{
    printf("Inside the Consumer Method");
    for(i = 0; i < 100; i++)
    {
        
        if(buffer->lower < buffer->upper)
        {
            printf("Number = %d\n", buffer->buffer[i]);
            buffer->buffer[i] = NULL;
            buffer->count--;
            buffer->lower = i;
        }
    }

    for(i = 0; i < 200; i++)
    {
        
        if(buffer->lower < buffer->upper)
        {
            printf("Number = %d\n", buffer->buffer[i]);
            buffer->buffer[i] = NULL;
            buffer->count--;
            buffer->lower = i;
        }
        else
        {
            printf("Lower is above upper cant consume");
        }
    }

    _exit(0);
}

void signal_handler(int signal_number)
{
    printf("Inside Signal Handler\n");
    if(buffer->count == 0)
    {
        producer();
    }
    else if (buffer->count > 0)
    {
       consumer();
    }
    else
    {
        printf("Not ready to consume. Please try again later");
    }
}

/**
 * Main application entry point.
 *
 * @return 1 if error or 0 if OK returned to code the caller.
 */
int main(int argc, char* argv[])
{
    
    pid_t  pid;

     // Create shared memory for the Circular Buffer to be shared between the Parent and Child  Processes
    buffer = (struct CIRCULAR_BUFFER*)mmap(0,sizeof(buffer), PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    buffer->count = 0;
    buffer->lower = 0;
    buffer->upper = 0;
    signal(SIGUSR1, signal_handler);
    pid = fork();
    if (pid == -1)
    {
        // Error: If fork() returns -1 then an error happened (for example, number of processes reached the limit).
        printf("Can't fork, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) // Child process will take this path
    {

    }
    else  // Parent process will take this path
    {
        raise(SIGUSR1);
        raise(SIGUSR1);
    }

    wait(0);

    // Return OK
    return 0;
}

/*
TODO:
     1) Write 0 to 99 in the buffer and print out each value in producer.
     
     2) compile and run it to make sure you are printing numbers. To print to
     the buffer you will use *(buffer->buffer + i) = i in a loop

     3) Once you get step 2 working, read all locations in the buffer and print out
     each value in consumer. You will use *(buffer->buffer + i) in a loop to read the
     data in the buffer.

     4) Compile and run it to make sure both the writes and reads are working (i.e. producer
     and concumer functions). At this point, nothing is controlling access to the buffer. You
     may see all of the produce messages and then all of the consume message or you may see
     a mix. As long as you are seeing reasonable looking data you should be able to continue.

     5) Add a second loop so you produce 0-99 twice and you read 0-99 twice.

     6) Compile and run it to make sure both the writes and reads are working (i.e. producer
     and concumer functions). At this point, nothing is controlling access to the buffer. You
     may see all of the produce messages and then all of the consume message or you may see
     a mix. As long as you are seeing reasonable looking data you should be able to continue.

     7) Add a signal so the producer writes 0-99, producer signals the consumer, the consumer reads 0-99,
     signals the producer, the producer writes 0-99 (or 100-199 whatever you want), producer signals
     the consumer, and the consumer reads 0-99 (or 100-199). You have examples with two signals, you only
     have to have 1 signal (SIGUSR1 or SIGUSR2, your choice) to make this work.

     Now, you are probably have an assignment somewhere in the mid-80s. Start adding the fancy stuff if you
     are not tired of the assignment yet.

     Below is some sort of code sort of commentary to help think about how to use the circular buffer.
     If this doesn't make sense to you, ignore it.

     Let's say the producer runs and wrote 4 items to buffer 
     buffer[0] = 0;
     buffer[1] = 1;
     buffer[2] = 2;
     buffer[3] = 3;

     buffer->count is 4
     buffer->lower is 0 (last index read so it starts off as 0)
     buffer->upper is 3 last index written

     some or all of these may be off by 1, I didn't code it up to find out. 

     when theconsumer runs 
     for (int i = 0; i < count; ++i)
     {
         printf("consumer read %d\n", *(buffer->buffer + buffer->lower++)); // This uses lower and then increments it

         Things to think about for additional logic

         make sure lower doesn't go past upper

         make sure upper doesn't go past lower

         keep in mind is the wrapping of the buffer (i.e. you can write [99] but the next write has to be [0])
         the wrapping makes it "circular"

         Because of buffer wrapping, the upper can be less than the lower so a simple comparison may not work
         e.g. lower can be 50 and upper can be 30. This means that [51]-[99] and [0]-[29] were written to
         by the producer and have data that needs to be read.

     }

     If all of that is working, you can manipulate your two loops in steps 1 and 5 to show the producer cannot
     overrun the consumer and the consumer canonot overrun the producer.
*/