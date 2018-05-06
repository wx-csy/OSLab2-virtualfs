#include <os.h>
#include <stdio.h>
#include <string.h>

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

static void blockme() {

}

static void wakeup(thread_t *thread) {
  
}

static void kmt_init() {

}

static int kmt_create(thread_t *thread, void (*entry)(void *arg),
    void *arg) {
  return 0;
}

static void kmt_teardown(thread_t *thread) {

}

static thread_t *kmt_schedule() {
  return NULL;
}

static void kmt_spin_init(spinlock_t *lk, const char *name) {
  _intr_write(0);
  if (name == NULL) name = "(anon)";
  strncpy(lk->name, name, sizeof lk->name);
  lk->name[sizeof(lk->name) - 1] = 0;
  lk->locked = 0;
  _intr_write(1);
}

static void kmt_spin_lock(spinlock_t *lk) {
  _intr_write(0);
  if (lk->locked) {
    printf("Fatal error occured.\n");
    printf("Attempting to acquire a locked spinlock [%s].\n", lk->name);
    _Exit(0);
  }
}

static void kmt_spin_unlock(spinlock_t *lk) {
  if (!lk->locked) {
    printf("Fatal error occured.\n");
    printf("Attempting to release an unlocked spinlock [%s].\n", 
        lk->name);
    _Exit(0);
  }
  _intr_write(1);
}

static void kmt_sem_init(sem_t *sem, const char *name, int value) {
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
  _intr_write(1);
}

static void kmt_sem_wait(sem_t *sem) {
  _intr_write(0);
  sem->value--;
  if (sem->value < 0) {
    thread *last = sem->next;
    sem->next = this_thread;
    
    sem->next = last;
  }
  _intr_write(1);
}

static void kmt_sem_signal(sem_t *sem) {
  _intr_write(0);
  sem->value++;
  if (sem->value <= 0) {
    
  }
  _intr_write(1);
}

