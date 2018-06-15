#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <am.h>
#include <nanos.h>
#include <pmr.h>

MODULE {
  void (*init)();
  void (*run)();
  _RegSet *(*interrupt)(_Event ev, _RegSet *regs);
} MOD_NAME(os);

MODULE {
  void (*init)();
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
} MOD_NAME(pmm);

typedef struct thread thread_t;
typedef struct spinlock spinlock_t;
typedef struct semaphore sem_t;

#define NR_THREADS    32
thread_t *threads[NR_THREADS];

MODULE {
  void (*init)();
  int (*create)(thread_t *thread, void (*entry)(void *arg), void *arg);
  void (*teardown)(thread_t *thread);
  thread_t *(*schedule)();
  void (*spin_init)(spinlock_t *lk, const char *name);
  void (*spin_lock)(spinlock_t *lk);
  void (*spin_unlock)(spinlock_t *lk);
  void (*sem_init)(sem_t *sem, const char *name, int value);
  void (*sem_wait)(sem_t *sem);
  void (*sem_signal)(sem_t *sem);
} MOD_NAME(kmt);

typedef struct filesystem filesystem_t;
typedef struct file file_t;
typedef int inode_t;

MODULE {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem_t *fs);
  int (*unmount)(const char *path);
  int (*fsls)(int (*fn)(const char *path, filesystem_t *fs));
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, const void *buf, size_t nbyte);
  off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
} MOD_NAME(vfs);

// filesystem

struct filesystem {
  
  Interface(filesystem)
    int     (*_ctor)  Member (const char *name);
    int     (*walk)   Member 
      (int (*fn)(const char *path, inode_t inode, int mode, int length));
    inode_t (*lookup) Member (const char *path);
    inode_t (*create) Member (const char *path);
    int     (*access) Member (inode_t inode, int mode);
    file_t* (*open)   Member (inode_t inode, int flags);
  End_Interface 
  
  char name[16];
  int refcnt;

};


// file

struct file {

  Interface(file)
    int     (*_ctor)  Member (filesystem_t *fs, inode_t inode, int flags);
    ssize_t (*read)   Member (char *buf, size_t size);
    ssize_t (*write)  Member (const char *buf, size_t size);
    off_t   (*lseek)  Member (off_t offset, int whence);
  End_Interface
  
  filesystem_t *fs;
  inode_t inode;
  int refcnt;
  int offset;
  int flags;

};

#endif 

