#include <kernel.h>
#include <os.h>
#include <test.h>
#include <pmr.h>
#include <fs/kvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define COUNT_N   300000

static spinlock_t io_lock;
static sem_t sem;

static thread_t workers[8];
static int counts[8];
static char files[4][64];

void worker(int* count) {
  int fds[4];
  int wid = *count;
  *count = 0;
  for (int i = 0; i < 4; i++) {
    fds[i] = vfs->open(files[i], O_RDWR);
  }
  for (int i = 0; i < COUNT_N; i++) {
    int fn = i % 4;
    vfs->lseek(fds[fn], 4 * ((fn + wid) % 8), SEEK_SET);
    vfs->write(fds[fn], &wid, 4);
    int val; 
    vfs->lseek(fds[fn], -4, SEEK_CUR);
    vfs->read(fds[fn], &val, 4);
    if (val == wid) (*count)++;
  }
  for (int i = 0; i < 4; i++) {
    vfs->close(fds[i]);
  }

  kmt->spin_lock(&io_lock);
  printf("[%d] done (wid = %d, count = %d)\n", this_thread->tid, wid, *count);
  kmt->spin_unlock(&io_lock);
    
  kmt->sem_signal(&sem);

  while (1);
};

int test_concrw() {
  puts("====== Concurrent R/W Test ======");
  puts("# test thread safety of kvfs");
  
  kmt->spin_init(&io_lock, "concrw.io_lock");
  kmt->sem_init(&sem, "concrw.sem", 0);
  
  srand(time(NULL));
  sh_mount("/concrw_tmp/", New(kvfs, "concrw_temp"));
  for (int i = 0; i < 4; i++) {
    sprintf(files[i], "/concrw_tmp/%x", rand());
    printf("file[%d]: %s\n", i, files[i]);
  }

  for (int i = 0; i < 8; i++) {
    counts[i] = i;
    sh_create_thread(&workers[i], (void *)worker, &counts[i]);
  }
  
  for (int i = 0; i < 8; i++) {
    kmt->sem_wait(&sem);
  }
  
  for (int i = 0; i < 8; i++) {
    sh_teardown(&workers[i]);
  }

  shb_ls();
  
  for (int i = 0; i < 8; i++) {
    if (counts[i] != COUNT_N) 
      VERDICT(1, "worker %d gives wrong answer: expected %d, found %d", 
          i, COUNT_N, counts[i]);
  }

  int numbers[8];
  for (int i = 0; i < 4; i++) {
    int fd = vfs->open(files[i], O_RDONLY);
    vfs->read(fd, &numbers[0], sizeof numbers);
    printf("file[%d] (%s): ", i, files[i]);
    for (int i = 0; i < 8; i++) printf("%d ", numbers[i]); puts("");
    int exp;
    for (int j = 0; j < 8; j++) {
      exp = (8 - i + j) % 8;
      if (numbers[j] != exp) {
        VERDICT(1, "%d-th number in %d-th file differ: expected %d, found %d",
            j, i, exp, numbers[i]);
      }
    }
    vfs->close(fd);
  }

  sh_unmount("/concrw_tmp/");
  VERDICT(0, "all values are correct");
}

