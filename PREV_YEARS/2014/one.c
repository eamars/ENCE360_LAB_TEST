#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_THREADS 8


pthread_mutex_t mutex;
int balance;

int getBalance() // java style, setter and getter
{
    return balance;
}

void setBalance(int b)
{
    balance = b;
}


void *make_withdrawal(void *ptr)
{
    // aquire the lock to the account
    pthread_mutex_lock(&mutex);

    int amount = getBalance();
    printf("Balance before :%d dollars \n", amount);
    amount = amount - 10;
    setBalance(amount);

    // release lock
    pthread_mutex_unlock(&mutex);

    printf("Balance after: %d dollars \n", amount);

    return NULL;
}

int main(void)
{
    int i = 0;
    pthread_t thread[NUM_THREADS];

    // start with 100 dollars
    setBalance(100);

    // intialize mutex
    pthread_mutex_init(&mutex, NULL);

    // create some threads to withdraw
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&thread[i], NULL, make_withdrawal, NULL);
    }

    // wait for all threads to be done
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }

    printf("Final balance: %d dollars \n", getBalance());
    return 0;
}
