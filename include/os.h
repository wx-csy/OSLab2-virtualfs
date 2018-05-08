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

struct semaphore {
  char name[16];
  int value;
  thread_t *next;
};

extern thread_t *this_thread;

#define STACK_PROTECTOR_MAGIC1  0x723a3ba8
#define STACK_PROTECTOR_MAGIC2  0xf38da472

static inline void check_stack(thread_t *thread) {
  _Area stack = thread->stack;
  if (*(uint32_t*)(stack.start) != STACK_PROTECTOR_MAGIC1 ||
    *(uint32_t*)ptr_advance(stack.end, -sizeof(uint32_t)) 
    != STACK_PROTECTOR_MAGIC2) {
    printf("Fatal error occured.\n");
    printf("Stack corruption detected!\n");
    printf("tid=%d, stack area=[%p, %p)\n", thread->tid,
        stack.start, stack.end);
    _Exit(EXIT_FAILURE);
  }
}

#endif

