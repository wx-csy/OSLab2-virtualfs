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

struct thread {
  int tid;
  int status;
  _Area stack;
  _RegSet *regset;
};

struct spinlock {
  char name[16];
  thread_t *holder;
  int last_intr;
};

#define MAX_SEM_WAIT  31
struct semaphore {
  char name[16];
  thread_t *queue[MAX_SEM_WAIT + 1];
  int lpos, rpos;
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

#endif

