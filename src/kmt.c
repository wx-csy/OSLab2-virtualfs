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

static void kmt_init() {
  char s1[10] = "213", s2[10];
  strcpy(s2, s1);
  printf("%s", s2);
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

}

static void kmt_spin_lock(spinlock_t *lk) {

}

static void kmt_spin_unlock(spinlock_t *lk) {

}

static void kmt_sem_init(sem_t *sem, const char *name, int value) {

}

static void kmt_sem_wait(sem_t *sem) {

}

static void kmt_sem_signal(sem_t *sem) {

}

