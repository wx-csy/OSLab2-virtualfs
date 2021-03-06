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
static spinlock_t cnt_lock;
static volatile int cnt;

static void worker(void *ignore) {
  while (1) {
    kmt->spin_lock(&cnt_lock);
    if (cnt < 10000000) cnt++; else {
      kmt->spin_unlock(&cnt_lock);
      break;
    }
    kmt->spin_unlock(&cnt_lock);
  } 

  kmt->spin_lock(&io_lock);
  printf("[%d] done, cnt=%d\n", this_thread->tid, cnt);
  kmt->spin_unlock(&io_lock);

  kmt->sem_signal(&sem);
  while (1); 
}

static thread_t workers[6];

int test_mttest2() {
  puts("====== Multi-threading Test (2) ======");
  puts("# counting 10000000 with 6 parallel threads");
  kmt->spin_init(&io_lock, "mttest2.io_lock");
  kmt->sem_init(&sem, "mttest2.sem", 0);
  kmt->spin_init(&cnt_lock, "mttest2.cnt");
  cnt = 0;
  for (int i = 0; i < 6; i++) 
    sh_create_thread(&workers[i], (void (*)(void *))worker, NULL);
  for (int i = 0; i < 6; i++) {
    char path[256]; sprintf(path, "/proc/%d", workers[i].tid);
    shb_type(path);
  }
  for (int i = 0; i < 6; i++)
    kmt->sem_wait(&sem);
  for (int i = 0; i < 6; i++) 
    sh_teardown(&workers[i]);

  if (cnt == 10000000) {
    VERDICT(0, "expected 10000000, found %d", cnt);
  } else {
    VERDICT(1, "expected 10000000, found %d", cnt);
  }
}

