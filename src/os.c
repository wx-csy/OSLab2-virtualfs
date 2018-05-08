#include <os.h>
#include <assert.h>
#include <stdio.h>

#define DEBUG_ME
#include <debug.h>

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
  while (1) _yield(); 
}

sem_t sem_free, sem_full;
spinlock_t spinlck;

#define SIZESIZE 4

void working(int t) {
  static int count = 0;
  kmt->spin_lock(&spinlck);
  if (t) {
    printf("(");
    count++;
  } else {
    printf(")");
    count--;
  }
  assert(count >= 0 && count <= SIZESIZE);
  kmt->spin_unlock(&spinlck);
}

void worker1(void *ignore) {
  while (1) {
    kmt->sem_wait(&sem_free);
    working(1);
    kmt->sem_signal(&sem_full);
    if (rand() % 2) _yield();
  }
}

void worker2(void *ignore) {
  while (1) {
    kmt->sem_wait(&sem_full);
    working(0);
    kmt->sem_signal(&sem_free);
    if (rand() % 2) _yield();
  } 
}

static void os_run() {
  thread_t thrd_idle;
  thread_t prod[6], cons[6]; 
  _intr_write(0);
  this_thread = NULL;
  kmt->sem_init(&sem_free, "sem_free", SIZESIZE);
  kmt->sem_init(&sem_full, "sem_full", 0);
  kmt->spin_init(&spinlck, "spinlck");
  kmt->create(&thrd_idle, idle, NULL);
  for (int i=0; i<6; i++) {
    kmt->create(prod+i, worker1, NULL);
    kmt->create(cons+i, worker2, NULL);
  }
  _intr_write(1);
  _yield();
  assert(0);
  // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
// _debug("Interrupted. tid=%d, ev=%d", this_thread?this_thread->tid:0,
//    ev.event);
//  if (ev.event == _EVENT_IRQ_TIMER) _putc('*');
  if (ev.event == _EVENT_IRQ_IODEV) _putc('I');
  if (ev.event == _EVENT_YIELD) {
// _debug("Thread %d yields!\n", this_thread->tid);
  }
  if (ev.event == _EVENT_ERROR) {
    printf("Error event received\n");
//    _putc('x');
    _halt(1);
  }
  if (this_thread != NULL) this_thread->regset = regs;
  this_thread = kmt->schedule(); 
  return this_thread->regset; // this is allowed by AM
}

