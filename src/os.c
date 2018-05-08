#include <os.h>
#include <assert.h>
#include <stdio.h>

// #define DEBUG_ME
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

#pragma GCC optimize(0)

static void overflow(int x) {
  if (x % 10000 == 0) _yield();
  overflow(x+1);
}

static void idle(void *ignore) {
  overflow(0);
  while (1) idle(NULL); 
}

static thread_t thrd_idle;
static void os_run() {
  kmt->create(&thrd_idle, idle, NULL);
  _yield();
  assert(0);
  // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
  switch (ev.event) {
    case _EVENT_IRQ_TIMER:
      break;
    case _EVENT_IRQ_IODEV:
      break;
    case _EVENT_YIELD:
      break;
    case _EVENT_ERROR:
      
      break;
    default:
      break;
  }
  if (this_thread != NULL) this_thread->regset = regs;
  this_thread = kmt->schedule(); 
  assert(this_thread->status == THRD_STATUS_RUNNING);
  return this_thread->regset; // this is allowed by AM
}

