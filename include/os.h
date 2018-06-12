#ifndef __OS_H__
#define __OS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

static inline void *ptr_advance(void *ptr, ptrdiff_t offset) {
  return (void*)((uint8_t*)ptr + offset);
}

#define THRD_STATUS_INVALID     0
#define THRD_STATUS_READY       1
#define THRD_STATUS_RUNNING     2
#define THRD_STATUS_BLOCKED     3

#define MAX_FD  16;

struct thread {
  int tid;
  int status;
  int intr_cnt;
  _Area stack;
  _RegSet *regset;
  file_t *fd[MAX_FD];
};

struct spinlock {
  int magic;
  char name[16];
  thread_t *holder;
};

struct semaphore {
  int magic;
  char name[16];
  struct spinlock lock;
  int value;
};

extern thread_t *this_thread;

#define STACK_PROTECTOR_MAGIC1  0x723a3ba8
#define STACK_PROTECTOR_MAGIC2  0xf38da472

static inline void check_stack(thread_t *thread) {
  _Area stack = thread->stack;
  if (*(uint32_t*)(stack.start) != STACK_PROTECTOR_MAGIC1 ||
    *(uint32_t*)ptr_advance(stack.end, -sizeof(uint32_t)) 
    != STACK_PROTECTOR_MAGIC2) {
    panic("Stack overflow detected.\n"
        "tid=%d, stack area=[%p, %p)\n", thread->tid,
        stack.start, stack.end);
  }
}

static inline void inc_intr(thread_t *thread) {
  _intr_write(0);
  thread->intr_cnt++;
}

static inline void dec_intr(thread_t *thread) {
  if (--(thread->intr_cnt) == 0) {
    _intr_write(1);
  } 
}

#endif

