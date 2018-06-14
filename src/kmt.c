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

#define MAX_THREAD_NUM  32
thread_t *threads[MAX_THREAD_NUM];

static void kmt_init() {
  for (int i = 0; i < MAX_THREAD_NUM; i++) {
    threads[i] = NULL;
  }
  this_thread = NULL;
}

#define STACK_SIZE  4 * 1024 * 1024
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
      thread->intr_cnt = 0;
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
  if (!succ) {
    panic("Creating new thread failed!\n"
        "Probably due to lack of memory");
  }
  return succ;
}

static void kmt_teardown(thread_t *thread) {
  int last_intr = _intr_read();
  _intr_write(0);
  thread->status = THRD_STATUS_INVALID;
  pmm->free(thread->stack.start);
  threads[thread->tid] = NULL;
  if (thread == this_thread) _yield();
  _intr_write(last_intr);
}

static thread_t *kmt_schedule() {
//  static int ntid = 0;
  int inittid = rand () % MAX_THREAD_NUM;
  int ntid = inittid;
  if (this_thread->status == THRD_STATUS_RUNNING) {
    this_thread->status = THRD_STATUS_READY;
  }
  while (threads[ntid]->status != THRD_STATUS_READY) {
    ntid++;
    ntid %= MAX_THREAD_NUM;
    if (ntid == inittid) {
      panic("Kernel scheduler can not find a ready thread!");
    }
  }
  threads[ntid]->status = THRD_STATUS_READY;
  this_thread = threads[ntid];
  if (this_thread->tid != 0)
_debug("Thread scheduled: tid=%d", this_thread->tid);
  this_thread->status = THRD_STATUS_RUNNING;
  check_stack(this_thread);
  return this_thread;
}

#define SPIN_MAGIC  0x8732f8a1
static void kmt_spin_init(spinlock_t *lk, const char *name) {
  int last_intr = _intr_read();
  _intr_write(0);
  if (name == NULL) name = "(anon)";
  lk->magic = SPIN_MAGIC;
  strncpy(lk->name, name, sizeof lk->name);
  lk->name[sizeof(lk->name) - 1] = 0;
  lk->holder = NULL;
  _intr_write(last_intr);
}

static void kmt_spin_lock(spinlock_t *lk) {
  inc_intr(this_thread);
_debug("lock[%s], tid=%d", lk->name, this_thread->tid);
  assert(lk->magic == SPIN_MAGIC);
  if (lk->holder != NULL) {
    panic("Attempting to acquire a locked spinlock [%s].\n", lk->name);
    _Exit(0);
  }
  lk->holder = this_thread;
}

static void kmt_spin_unlock(spinlock_t *lk) {
_debug("unlock[%s], tid=%d", lk->name, this_thread->tid);
  assert(lk->magic == SPIN_MAGIC);
  if (lk->holder == NULL) {
    panic("Attempting to release an unlocked spinlock [%s].\n", 
        lk->name);
  }
  lk->holder = NULL;
  dec_intr(this_thread);
}

#define SEM_MAGIC   0xb128c183
static void kmt_sem_init(sem_t *sem, const char *name, int value) {
  kmt->spin_init(&(sem->lock), name);
  
  int last_intr = _intr_read();
  _intr_write(0);
  if (name == NULL) name = "(anon)";
  strncpy(sem->name, name, sizeof sem->name);
  sem->magic = SEM_MAGIC;
  sem->name[sizeof(sem->name) - 1] = 0;
  if (value < 0) {
    panic("Attempting to initialize semaphore [%s]" 
           "with negative value.\n", sem->name);
  }
  sem->value = value;
  _intr_write(last_intr);
}

static void kmt_sem_wait(sem_t *sem) {
  kmt->spin_lock(&(sem->lock));
  
  assert(sem->magic == SEM_MAGIC);
  
  while (sem->value <= 0) {
    kmt->spin_unlock(&(sem->lock));
    _yield();
    kmt->spin_lock(&(sem->lock));
  }
  sem->value--;
  
  kmt->spin_unlock(&(sem->lock));
}

static void kmt_sem_signal(sem_t *sem) {
  kmt->spin_lock(&(sem->lock));

  assert(sem->magic == SEM_MAGIC);
  
  sem->value++;

  kmt->spin_unlock(&(sem->lock));
}

