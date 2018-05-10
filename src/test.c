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

static void trash(void *ignore) {
  while (1) {
    if (rand() % 100 > 98)
    printf("t");
  }
}

static spinlock_t spnlck;
static int trash_stat[4];
static thread_t trash_th[4];

static void producer(const char* ch) {
  while (1) {
    kmt->sem_wait(&empty);
    printch(*ch, 1);
    kmt->sem_signal(&full);
  if (rand()%100 > 20) _yield();
    if (rand()%10 > 7) {
      kmt->spin_lock(&spnlck);
      int id = rand() % 4;
      if (trash_stat[id]) {
        kmt->teardown(trash_th + id);
      } else {
        kmt->create(trash_th + id, trash, NULL);
      }
      trash_stat[id] ^= 1;
      kmt->spin_unlock(&spnlck);
    }
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
  kmt->spin_init(&spnlck, "trash_spin");
  for (int i=0; i<12; i++) {
    kmt->create(cons_th + i, (void (*)(void*))consumer, ")"); 
    kmt->create(prod_th + i, (void (*)(void*))producer, "(");
  }
}

