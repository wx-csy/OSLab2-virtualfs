// C polymorphism support header

#ifndef __PMR_H__
#define __PMR_H__

#define __Allocate(size) (pmm->alloc(size))

#define __Deallocate(ptr) (pmm->free(ptr))

#define __GET_VTABLE_TYPE(pname) _##pname##_vtable_t

#define __GET_VTABLE_NAME(cname) _##cname##_vtable

#define Member(...) (void *_this, ##__VA_ARGS__)

#define MemberOf(cname) \
  struct cname *__this = _this; \
  do { \
    __this; \
  } while (0) 

#define this (*__this)

#define base (this._base)

#define Interface(pname) \
  const struct __GET_VTABLE_TYPE(pname) { 

#define End_Interface \
    void (*_dtor)(void *_this); \
  } *_vtable;

#define Inherits(pname) \
  struct pname _base

#define Implementation(pname, cname) \
  const struct __GET_VTABLE_TYPE(pname) __GET_VTABLE_NAME(cname)

#define Invoke(p_object, method, ...) \
  ((p_object)->_vtable->method((p_object), ##__VA_ARGS__))

#define New(cname, ...) ({ \
  struct cname *ptr = __Allocate(sizeof(struct cname)); \
  if (ptr != NULL) { \
    ptr->base._vtable = &__GET_VTABLE_NAME(cname); \
    if (ptr->base._vtable->_ctor) \
      if (ptr->base._vtable->_ctor((void *)ptr, ##__VA_ARGS__) != 0) { \
        __Deallocate(ptr); \
        ptr = NULL; \
      } \
  } \
  (void *)ptr; \
})

#define Delete(p_object) do { \
  if ((p_object)->_vtable->_dtor) { \
    (p_object)->_vtable->_dtor(p_object); \
  } \
  __Deallocate(p_object); \
} while (0)

#endif

 
