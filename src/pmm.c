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

static void *ptr_advance(void *ptr, ptrdiff_t offset) {
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

static size_t chk_start, chk_end;
static uint8_t *mem_table;

static void pmm_init() {
  chk_start = (uintptr_t)(align_up(_heap.start)) / CHUNK_SIZE;
  chk_end = (uintptr_t)(align_down(_heap.end)) / CHUNK_SIZE;
  mem_table = (uint8_t *)(align_up(_heap.start));
  chk_start += (chk_end + CHUNK_SIZE - 1) / CHUNK_SIZE;
  printf("chk_start=0x%x, chk_end=0x%x\n", chk_start, chk_end);
  if (chk_start >= chk_end) {
    puts("Lack of memory.");
    _Exit(EXIT_FAILURE);
  }
  memset(mem_table, 0, chk_end - chk_start);
  for (size_t chk_i = 0; chk_i < chk_start; chk_i++) 
    mem_table[chk_i] = -1;
  void *test = pmm_alloc(235264);
  printf("addr = %p\n", test);
  pmm_free(test);
}

static void *pmm_alloc(size_t size) {
  if (size == 0) return NULL;
  while (size & (size - 1)) size += size & -size;
  if (size < CHUNK_SIZE) size = CHUNK_SIZE;
  size_t chk_cnt = size / CHUNK_SIZE;
  uint8_t log_chk = 0;
  while (chk_cnt) {
    log_chk++;
    chk_cnt >>= 1;
  }
  chk_cnt = size / CHUNK_SIZE;
  size_t chk_pos = chk_start;
  while (chk_pos & (chk_cnt - 1)) chk_pos += chk_pos & -chk_pos;
  while (chk_pos + chk_cnt <= chk_end) {
    for (size_t chk_i = chk_pos; chk_i < chk_pos + chk_cnt; chk_i++) {
      if (mem_table[chk_i]) goto next;
    }  
    for (size_t chk_i = chk_pos; chk_i < chk_pos + chk_cnt; chk_i++) {
      mem_table[chk_i] = log_chk;
    }
    return ptr_advance(NULL, chk_pos * CHUNK_SIZE);
next:;
    chk_pos += chk_cnt;
  }
  return NULL;
}

static void pmm_free(void *ptr) {
  if (ptr == NULL) return;
  uintptr_t addr = ptr;
  size_t chk = addr / CHUNK_SIZE;
  if (addr & (CHUNK_SIZE - 1)) goto free_err;
  if (chk < chk_start || chk >= chk_end) goto free_err;
  uint8_t log_chk = mem_table[chk];
  size_t chk_cnt = (size_t)1 << (log_chk - 1);
  if (chk & (chk_cnt - 1)) goto free_err;
  if (chk + chk_cnt > chk_end) goto free_err;
  for (size_t chk_i = chk; chk_i < chk + chk_cnt; chk_i++) {
    if (mem_table[chk_i] != log_chk) goto free_err;
    mem_table[chk_i] = 0;
  }
  return;
free_err:
  printf("Failure occured when freeing pointer 0x%p!\n", ptr);
  printf("Printing current memory table ...\n");
  for (size_t chk_i = 0; chk_i < chk_end; chk_i++) {
    if (chk_i & 15 == 0)
      printf("\n0x%8x ", (unsigned)(chk_i * CHUNK_SIZE)); 
    printf("%2x ", mem_table[chk_i]);
  }
  _Exit(EXIT_FAILURE);
}

