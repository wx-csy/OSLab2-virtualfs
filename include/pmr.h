// C polymorphism support header

#ifndef __PMR_H__
#define __PMR_H__

#define __Allocate(size) (pmm->alloc(size))

#define __Deallocate(ptr) (pmm->free(ptr))

#define __GET_VTABLE_TYPE(pname) _##pname##_vtable_t

#define __GET_VTABLE_NAME(cname) _##cname##_vtable

#define Interface(pname) \
  const struct __GET_VTABLE_TYPE(pname) { 

#define End_Interface \
  } *_vtable;

#define Implementation(pname, cname) \
  const struct __GET_VTABLE_TYPE(pname) __GET_VTABLE_NAME(cname)

#define Invoke(p_object, method, ...) \
  ((p_object)->_vtable->method((p_object), ##__VA_ARGS__))

#define PMR_Init(p_object, cname) \
  ((p_object)->_vtable = &__GET_VTABLE_NAME(cname))

#define New(pname, cname) ({ \
  pname *ptr = pmm->alloc(sizeof(cname)); \
  if (ptr != NULL) { \
    PMR_Init(ptr, cname); \
    Invoke(ptr, _ctor); \
  } \
  (cname *) ptr; \
})

#endif

 
