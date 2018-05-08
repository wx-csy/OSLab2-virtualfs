#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os.h>

static void kmt_init();
static int kmt_create(thread_t *thread, void (*entry)(void *arg), 
    void *arg);
static void kmt_teardown(thread_t *thread);
static thread_t *kmt_schedule();
static void kmt_spin_init(spinlock_t *lk, const char *name);
static void kmt_spin_lock(spinlock_t *lk);
static void kmt_spin_unlock(spinlock_t *lk);
static void kmt_sem_init(sem_t *sem, const char *name, int value);
static void kmt_sem_wait(sem_t *sem);
static void kmt_sem_signal(sem_t *sem);

MOD_DEF(kmt) {
  .init = kmt_init,
  .create = kmt_create,
  .teardown = kmt_teardown,
  .schedule = kmt_schedule,
  .spin_init = kmt_spin_init,
  .spin_lock = kmt_spin_lock,
  .spin_unlock = kmt_spin_unlock,
  .sem_init = kmt_sem_init,
  .sem_wait = kmt_sem_wait,
  .sem_signal = kmt_sem_signal,
};

thread_t *this_thread;

static void blockme() {
  this_thread->status = THRD_STATUS_BLOCKED;
}

static void wakeup(thread_t *thread) {
  thread->status = THRD_STATUS_READY;
}

#define MAX_THREAD_NUM  32
thread_t *threads[MAX_THREAD_NUM];

static void kmt_init() {
  for (int i = 0; i < MAX_THREAD_NUM; i++) {
    threads[i] = NULL;
  }
}

#define STACK_SIZE  8 * 1024 * 1024
static int kmt_create(thread_t *thread, void (*entry)(void *arg),
    void *arg) {
  int last_intr = _intr_read();
  int succ = 0;
  _intr_write(0);
  _Area stack;
  for (int i = 0; i < MAX_THREAD_NUM; i++) {
    if (threads[i] == NULL) {
      stack.start = pmm->alloc(STACK_SIZE);
      if (stack.start == NULL) break;
      stack.end = ptr_advance(stack.start, STACK_SIZE);
      thread->stack = stack;
      *(uint32_t*)stack.start = STACK_PROTECTOR_MAGIC1;
      stack.start = ptr_advance(stack.start, sizeof(uint32_t));
      stack.end = ptr_advance(stack.end, -sizeof(uint32_t));
      *(uint32_t*)stack.end = STACK_PROTECTOR_MAGIC2;
      thread->tid = i;
      thread->regset = _make(stack, entry, arg);
      thread->status = THRD_STATUS_READY;
      threads[i] = thread;
      printf("create thread: tid=%d\n", thread->tid);
      succ = 1;
      break;
    }
  }
  _intr_write(last_intr);
  return succ;
}

static void kmt_teardown(thread_t *thread) {
  int last_intr = _intr_read();
  _intr_write(0);
  thread->status = THRD_STATUS_INVALID;
  pmm->free(thread->stack.start);
  threads[thread->tid] = NULL;
  this_thread = kmt_schedule();
  _intr_write(last_intr);
  _yield();
}

static thread_t *kmt_schedule() {
  static int ntid = 0;
  if (this_thread->status == THRD_STATUS_RUNNING) {
    this_thread->status = THRD_STATUS_READY;
  }
  do {
    ntid++;
    ntid %= MAX_THREAD_NUM;
  } while (threads[ntid]->status != THRD_STATUS_READY);
  threads[ntid]->status = THRD_STATUS_READY;
  this_thread = threads[ntid];
  check_stack(this_thread->stack);
  return this_thread;
}

static void kmt_spin_init(spinlock_t *lk, const char *name) {
  int last_intr = _intr_read();
  _intr_write(0);
  if (name == NULL) name = "(anon)";
  strncpy(lk->name, name, sizeof lk->name);
  lk->name[sizeof(lk->name) - 1] = 0;
  lk->holder = NULL;
  _intr_write(last_intr);
}

static void kmt_spin_lock(spinlock_t *lk) {
  lk->last_intr = _intr_read();
  _intr_write(0);
  if (lk->holder != NULL) {
    printf("Fatal error occured.\n");
    printf("Attempting to acquire a locked spinlock [%s].\n", lk->name);
    _Exit(0);
  }
  lk->holder = this_thread;
}

static void kmt_spin_unlock(spinlock_t *lk) {
  if (lk->holder != NULL) {
    printf("Fatal error occured.\n");
    printf("Attempting to release an unlocked spinlock [%s].\n", 
        lk->name);
    _Exit(0);
  }
  lk->holder = NULL;
  _intr_write(lk->last_intr);
}

static void kmt_sem_init(sem_t *sem, const char *name, int value) {
  int last_intr = _intr_read();
  _intr_write(0);
  if (name == NULL) name = "(anon)";
  strncpy(sem->name, name, sizeof sem->name);
  sem->name[sizeof(sem->name) - 1] = 0;
  if (value < 0) {
    printf("Fatal error occured!\n");
    printf("Attempting to initialize semaphore [%s]" 
           "with negative value.\n", sem->name);
    _Exit(0);
  }
  sem->value = value;
  sem->next = NULL;
  _intr_write(last_intr);
}

static void kmt_sem_wait(sem_t *sem) {
  int last_intr = _intr_read();
  _intr_write(0);
  sem->value--;
  if (sem->value < 0) {
    thread_t *last = sem->next;
    sem->next = this_thread;
    blockme();
    _intr_write(last_intr);
    _yield();
    last_intr = _intr_read();
    sem->next = last;
  }
  _intr_write(last_intr);
}

static void kmt_sem_signal(sem_t *sem) {
  int last_intr = _intr_read();
  _intr_write(0);
  sem->value++;
  if (sem->value <= 0) {
    wakeup(sem->next);
  }
  _intr_write(last_intr);
  _yield();
}

