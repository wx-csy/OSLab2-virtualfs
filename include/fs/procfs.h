#ifndef __FS__PROCFS_H__
#define __FS__PROCFS_H__

#include <kernel.h>
#include <pmr.h>

#define MAX_DEV   16

typedef struct procfs {
  Inherits(filesystem);
  
} procfs_t;

extern Implementation(filesystem, procfs);

typedef struct otrofile {
  Inherits(file);
  
  char *data;
} otrofile_t;

extern Implementation(file, otrofile);

#endif 
