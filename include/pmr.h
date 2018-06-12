// C polymorphism support header (through interface)

#ifndef __PMR_H__
#define __PMR_H__

#define __GET_VTABLE_TYPE(pname) _##pname##_vtable_t

#define __GET_VTABLE_NAME(cname) _##cname##_vtable

#define Interface(pname) \
  struct __GET_VTABLE_TYPE(pname) { 

#define End_Interface \
  } *_vtable;

#define Inherits(pname) \
  struct pname base; \
  struct __GET_VTABLE_TYPE(pname) *_vtable;

#define Implementation(pname, cname) \
  extern const struct __GET_VTABLE_TYPE(pname) __GET_VTABLE_NAME(cname)

#define Invoke(object, method, ...) \
  ((object)._vtable->method(&(object), ##__VA_ARGS__))

#define PInvoke(object, method, ...) \
  ((object)->_vtable->method((object), ##__VA_ARGS__))

#define PMR_Init(object, cname) ((object)._vtable = __GET_VTABLE_NAME(cname))

#endif

 
