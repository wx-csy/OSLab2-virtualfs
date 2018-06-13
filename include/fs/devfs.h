#ifndef __FS__DEVFS_H__
#define __FS__DEVFS_H__

#include <kernel.h>
#include <pmr.h>

#define MAX_DEV   16

typedef struct devfs {
  struct filesystem base;

  struct {
    int valid;
    char name[32];
    int mode;
    char (*getch)();
    void (*putch)(char ch);
  } devices[MAX_DEV];

} devfs_t;

extern Implementation(filesystem, devfs);

extern Implementation(file, devfile);

#endif 
