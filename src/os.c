#include <os.h>
#include <assert.h>
#include <stdio.h>
#include <fs/devfs.h>
#include <fs/kvfs.h>
#include <fs/procfs.h>

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

static void init_thrd(void *ignore) {
  vfs->mount("/dev/", New(devfs, "devfs"));
  vfs->mount("/proc/", New(procfs, "procfs"));
  vfs->mount("/", New(kvfs, "kvfs"));

  void devfs_test(void *ignore);
  devfs_test(NULL); 
  while (1) _yield();
}

static thread_t thrd_idle;
static void os_run() {
  kmt->create(&thrd_idle, init_thrd, NULL);
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
      panic("Error event occurred!"); 
      break;
    default:
      panic("Unrecognized interruption (%d).", ev.event);
      break;
  }
  if (this_thread != NULL) this_thread->regset = regs;
  this_thread = kmt->schedule(); 
  assert(this_thread->status == THRD_STATUS_RUNNING);
  return this_thread->regset;
}

