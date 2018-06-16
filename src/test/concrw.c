#include <kernel.h>
#include <os.h>
#include <test.h>
#include <pmr.h>
#include <fs/kvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static spinlock_t io_lock;
static sem_t sem;

static thread_t workers[8];
static int counts[8];
static const char files[4][64];

void worker(int *count) {
  int fds[4];
  *count = 0;
  for (int i = 0; i < 4; i++) {
    fds[i] = vfs->open(files[i], O_RDWR);
  }
  int tid = this_thread->tid;
  for (int i = 0; i < 200000; i++) {
    int fn = i % 4;
    vfs->lseek(fds[fn], 4 * ((fn + tid) % 8), SEEK_SET);
    vfs->write(fds[fn], &tid, 4);
    int val; 
    vfs->lseek(fds[fn], -4, SEEK_CUR);
    vfs->read(fds[fn], &val, 4);
    if (val == tid) (*count)++;
  }
  for (int i = 0; i < 4; i++) {
    vfs->close(fds[i]);
  }

  kmt->spin_lock(&io_lock);
  printf("[%d] done (count = %d)\n", tid, *count);
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
    sh_create_thread(&workers[i], (void *)worker, &counts[i]);
  }
  
  for (int i = 0; i < 8; i++) {
    kmt->sem_wait(&sem);
  }

  shb_ls();
  sh_unmount("/concrw_tmp/");

  VERDICT(0, "numbers are correct");
}

