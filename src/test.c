#include <os.h>
#include <stdio.h>
#include <stdlib.h>
#define SEM_SZ 6

static sem_t full, empty;

static void printch(char ch, int id) {
  static int cnt = 0;
  printf("%c", ch);
  if (id) cnt++; else cnt--;
  assert(cnt >= 0 & cnt <= sem_sz);
}

static void producer(const char* ch) {
  while (1) {
    kmt->sem_wait(&empty);
    printch(*ch, 1);
    kmt->sem_post(&full);
  }
}

static void consumer(const char* ch) {
  while (1) {
    kmt->sem_wait(&full);
    printch(*ch, 0);
    kmt->sem_wait(&empty);
  }
}

thrd_t prod_th[6], cons_th[6];
void test() {
  kmt->sem_init(full, "sem_full", 0);
  kmt->sem_init(empty, "sem_empty", 12);
  for (int i=0; i<12; i++) {
    kmt->create(prod_th + i, producer, "(");
    kmt->create(consumer_th + i, consumer, ")"); 
  }
}

