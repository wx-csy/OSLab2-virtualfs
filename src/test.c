#include <os.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define SEM_SZ 6

static sem_t full, empty;

static void printch(char ch, int id) {
  static int cnt = 0;
  printf("%c", ch);
  if (id) cnt++; else cnt--;
  if (!(cnt >= 0 && cnt <= SEM_SZ)) {
    puts("Semaphore assertion failed!");
    printf("full.value=%d, empty.value=%d\n", full.value, empty.value);
    printf("cnt=%d, tid=%d\n", this_thread->tid);
    _Exit(0);
  }
}

static void producer(const char* ch) {
  while (1) {
    kmt->sem_wait(&empty);
    printch(*ch, 1);
    kmt->sem_signal(&full);
  if (rand()%100 > 20) _yield();
  }
}

static void consumer(const char* ch) {
  while (1) {
    kmt->sem_wait(&full);
    printch(*ch, 0);
    kmt->sem_signal(&empty);
  if (rand()%100 > 20) _yield();
  }
}

static thread_t prod_th[12], cons_th[12];
void test() {
  srand(time(NULL));
  kmt->sem_init(&full, "sem_full", 0);
  kmt->sem_init(&empty, "sem_empty", SEM_SZ);
  for (int i=0; i<12; i++) {
    kmt->create(cons_th + i, (void (*)(void*))consumer, ")"); 
    kmt->create(prod_th + i, (void (*)(void*))producer, "(");
  }
}

