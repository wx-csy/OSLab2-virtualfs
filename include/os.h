#ifndef __OS_H__
#define __OS_H__

#include <kernel.h>

#define THRD_STATUS_INVALID     0
#define THRD_STATUS_READY       1
#define THRD_STATUS_RUNNING     2
#define THRD_STATUS_BLOCKED     3

struct thread {
  int status;
};

struct spinlock {
  char name[16];
  int locked;
};

struct semaphore {
  char name[16];
  int value;
  thread_t *next;
};

extern thread_t *this_thread;

#endif
