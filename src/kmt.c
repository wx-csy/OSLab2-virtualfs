#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <os.h>

#define DEBUG_ME
#include <debug.h>

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
_debug("current thread (tid=%d) blocked\n", this_thread->tid);
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
      stack = thread->stack;
_debug("tid=%d, area=[%p, %p), regset=%p, status=%d", 
    thread->tid, stack.start, stack.end, thread->regset, thread->status);
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
  if (this_thread->tid != 0)
_debug("Thread scheduled: tid=%d", this_thread->tid);
  this_thread->status = THRD_STATUS_RUNNING;
  check_stack(this_thread);
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
_debug("[%s], tid=%d", lk->name, this_thread->tid);
  if (lk->holder != NULL) {
    printf("Fatal error occured.\n");
    printf("Attempting to acquire a locked spinlock [%s].\n", lk->name);
    _Exit(0);
  }
  lk->holder = this_thread;
}

static void kmt_spin_unlock(spinlock_t *lk) {
_debug("[%s], tid=%d", lk->name, this_thread->tid);
  if (lk->holder == NULL) {
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
  sem->lpos = sem->rpos = 0;
  sem->value = value;
  _intr_write(last_intr);
}

static void kmt_sem_wait(sem_t *sem) {
  int last_intr = _intr_read();
  _intr_write(0);
  sem->value--;
_debug("P[%s], value=%d, tid=%d", sem->name, sem->value, 
    this_thread->tid);
  if (sem->value < 0) {
    sem->queue[sem->rpos] = this_thread;
    sem->rpos = (sem->rpos + 1) & MAX_SEM_WAIT;
    if (sem->lpos == sem->rpos) {
      assert(0);
    }
    blockme();
    _yield();
  }
  _intr_write(last_intr);
}

static void kmt_sem_signal(sem_t *sem) {
  int last_intr = _intr_read();
  _intr_write(0);
  sem->value++;
_debug("V[%s], value=%d, tid=%d", sem->name, sem->value, 
      this_thread->tid);
  if (sem->value <= 0) {
    wakeup(sem->queue[sem->lpos]);
    sem->lpos = (sem->lpos + 1) & MAX_SEM_WAIT;
  }
  _intr_write(last_intr);
}

