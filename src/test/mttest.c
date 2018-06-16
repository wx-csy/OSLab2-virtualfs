#include <kernel.h>
#include <os.h>
#include <test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

static spinlock_t io_lock;
static sem_t sem;

int is_prime(int x) {
  for (int i = 2; i * i < x; i++) {
    if (x % i == 0) return 0;
  }
  return 1;
}

static void worker(int *result) {
  int cnt = 0;
  int last_prime;
  for (int i = 2; cnt < 10000; i++) {
    if (is_prime(i)) {
      cnt++;
      last_prime = i;
    }
  }
  *result = last_prime;
  kmt->spin_lock(&io_lock);
  printf("[%x] the 10000th prime is %s\n", this_thread->tid, last_prime);
  kmt->spin_unlock(&io_lock);
  
  kmt->sem_signal(&sem);
  while (1); 
}

thread_t workers[4];
int results[4];

int test_mttest() {
  kmt->spin_init(&io_lock, "mttest.io_lock");
  kmt->sem_init(&sem, "mttest.sem", 0);
  for (int i = 0; i < 4; i++) 
    sh_create_thread(&workers[i], (void *)worker, &results[i]);
  for (int i = 0; i < 4; i++)
    kmt->sem_wait(&sem);
  VERDICT(0, "numbers are correct");
  return 0;
}

