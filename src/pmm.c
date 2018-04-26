#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <os.h>

static void pmm_init();
static void *pmm_alloc(size_t size);
static void pmm_free(void *ptr);

MOD_DEF(pmm) {
  .init = pmm_init,
  .alloc = pmm_alloc,
  .free = pmm_free  
};

#define CHUNK_SIZE    512

void *ptr_advance(void *ptr, ptrdiff_t offset) {
  return (void*)((uint8_t*)ptr + offset);
}

static inline void *align_up(void *ptr) {
  uintptr_t addr = (uintptr_t)ptr;
  while (addr & (CHUNK_SIZE - 1)) addr += addr & -addr;
  return (void*)addr;
}

static inline void *align_down(void *ptr) {
  uintptr_t addr = (uintptr_t)ptr;
  while (addr & (CHUNK_SIZE - 1)) addr -= addr & -addr;
  return (void*)addr;
}

static void *start, *end;
size_t chk_start, chk_end;

uint8_t *mem_table;

static void pmm_init() {
  start = align_up(_heap.start);
  end = align_down(_heap.end);
  chk_start = (uintptr_t)start / CHUNK_SIZE;
  chk_end = (uintptr_t)end / CHUNK_SIZE;
  mem_table = (uint8_t *)start;
  chk_start += (chk_end - chk_start) / CHUNK_SIZE;
  start = (void *)(chk_start * CHUNK_SIZE);
  end = (void *)(chk_end * CHUNK_SIZE);
  memset(mem_table, 0, chk_end - chk_start);
  if (chk_start >= chk_end) {
    puts("Lack of memory.");
    _Exit(EXIT_FAILURE);
  }
}

static void *pmm_alloc(size_t size) {
  return 0;
}

static void pmm_free(void *ptr) {
  return;
}

