/*
Same question but I used semaphore to sync read and write

Example output:

Setting data	Data = 1804289383
Setting data	Data = 846930886
Setting data	Data = 1681692777
Setting data	Data = 1714636915
Setting data	Data = 1957747793
exiting


*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t sem_read;
sem_t sem_write;

int global_data = 0;

void read_data();
void set_data();

int main(void)
{
    pthread_t thread1, thread2;
    int thread_return1, thread_return2;
    int i;

    sem_init(&sem_read, 0, 0);
    sem_init(&sem_write, 0, 1);

    for (i = 0; i < 5; i++)
    {
        thread_return1 = pthread_create(&thread1, NULL, (void *)&set_data, NULL);
        thread_return2 = pthread_create(&thread2, NULL, (void *)&read_data, NULL);
    }

    // wait until two threads complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&sem_write);
    sem_destroy(&sem_read);


    printf("exiting\n");
    exit(0);

}

void set_data()
{
    sem_wait(&sem_write);
    printf("Setting data\t");
    global_data = rand();
    sem_post(&sem_read);
}

void read_data()
{
    int data;

    sem_wait(&sem_read);
    data = global_data;
    printf("Data = %d\n", data);
    sem_post(&sem_write);
}
