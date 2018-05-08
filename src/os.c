#include <os.h>
#include <assert.h>
#include <stdio.h>
static void os_init();
static void os_run();
static _RegSet *os_interrupt(_Event ev, _RegSet *regs);

MOD_DEF(os) {
  .init = os_init,
  .run = os_run,
  .interrupt = os_interrupt,
};

static void os_init() {
  for (const char *p = "Hello, OS World!\n"; *p; p++) {
    _putc(*p);
  }
}

void idle(void *ignore) {
  while (1) 
    printf("Hello, world!\n");
}

sem_t sem_free, sem_full;

void worker1(void *ignore) {
  while (1) {
    kmt->sem_wait(&sem_free);
    putchar('(');
    kmt->sem_signal(&sem_full);
  }
}

void worker2(void *ignore) {
  while (1) {
    kmt->sem_wait(&sem_full);
    putchar(')');
    kmt->sem_signal(&sem_free);
  } 
}

static void os_run() {
  thread_t thrd_idle, thrd_worker, thrd_worker2;
  _intr_write(0);
  kmt->sem_init(&sem_free, "sem_free", 2);
  kmt->sem_init(&sem_full, "sem_full", 0);
  kmt->create(&thrd_idle, idle, NULL);
  kmt->create(&thrd_worker, worker, NULL);
  kmt->create(&thrd_worker2, worke2, NULL);
  printf("pid=%d, %d\n", thrd_idle.tid, thrd_worker.tid);
  _intr_write(1);
  _yield();
  assert(0);
  // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
//  if (ev.event == _EVENT_IRQ_TIMER) _putc('*');
//  if (ev.event == _EVENT_IRQ_IODEV) _putc('I');
  if (ev.event == _EVENT_ERROR) {
    printf("Error event received\n");
//    _putc('x');
    _halt(1);
  }
  this_thread = kmt->schedule();  
  return this_thread->regset; // this is allowed by AM
}

