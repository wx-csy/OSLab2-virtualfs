#include <kernel.h>
#include <os.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

static spinlock_t exec_lock;

#define ENTER   kmt->spin_lock(&exec_lock)
#define RETURN  kmt->spin_unlock(&exec_lock); return

#define SYSCALL_PRINT(fmt, ...) \
  printf("\033[93m" fmt "\n\033[0m", ##__VA_ARGS__)

void init_test() {
  kmt->spin_init(&exec_lock, "test.lock");
}

int sh_create_thread(thread_t *thread, void entry(void *arg), void *arg) {
  ENTER;

  int ret = kmt->create(thread, entry, arg);
  int pid = ret == 1 ? thread->pid : -1;

  SYSCALL_PRINT("create_thread(thread = %p, entry = %p, arg = %p) = %d" "
      (pid = %d)", thread, entry, arg, ret);

  RETURN ret;
}

void sh_teardown(thread_t *thread) {
  ENTER;

  SYSCALL_PRINT("teardown(thread = %p [tid = %d])", thread, thread->tid);
  
  kmt->teardown(thread);

  RETURN;
}
