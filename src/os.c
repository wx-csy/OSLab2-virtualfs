#include <os.h>
#include <assert.h>

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

void idle(void *i) {
  while (1) 
    printf("%d\n", *(int*)i);
}

int tmp =  1;

static void os_run() {
  _intr_write(1); // enable interrupt
  thread_t thrd_idle;
  kmt->create(&thrd_idle, idle, &tmp);
  _yield();
  assert(0);
  // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
  if (ev.event == _EVENT_IRQ_TIMER) _putc('*');
  if (ev.event == _EVENT_IRQ_IODEV) _putc('I');
  if (ev.event == _EVENT_ERROR) {
    _putc('x');
    _halt(1);
  }
  this_thread = kmt->schedule();  
  return this_thread->regset; // this is allowed by AM
}
