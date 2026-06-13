#ifndef __VEC_H_
#define __VEC_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef VEC_DEFAULT_CAP
#define VEC_DEFAULT_CAP 3
#endif

struct vector {
  void *data;
  unsigned int type_sz;
  unsigned int len;
  unsigned int cap;
};

#define vector_for_each(__vptr, __i) \
  for ((__i) = (__vptr)->data; (__i) < (__typeof__(__i))((char*)(__vptr)->data + ((__vptr)->type_sz * (__vptr)->len)); (__i)++)

// constructors
#define vector(type) __vector(sizeof(type))
#define vector_with_capacity(type, cap) __vector_with_capacity(sizeof(type), cap)

struct vector __vector(unsigned int type_sz);
struct vector __vector_with_capacity(unsigned int type_sz, unsigned int cap);
struct vector vector_copy(struct vector*);
struct vector vector_move(struct vector*);

// deconstructor
void vector_delete(struct vector*);
void vector_delete_shared_ptr(void*);
void vector_delete_shared_ptr_p(void*);

// impl
void* vector_at(struct vector*, unsigned int idx);
void vector_reserve(struct vector*, unsigned int additional);
void vector_push(struct vector*, void *new);
void* vector_emplace(struct vector*);
void vector_remove(struct vector*, unsigned int idx);
void vector_swap_remove(struct vector*, unsigned int idx);

static inline bool vector_is_empty(struct vector *v) { return !v->len; }

// experimental

// how to use:
// have to copy content of the returned pointer immediately before a push call
// ```c
//  struct vector v = vector(int);
//  vector_push(&v, 5);
//  int i = *(int*)vector_pop(&v); // valid i is 5. note that this line will segfault if vector is empty. cuz it will return NULL
//  int *j = vector_pop(&v);
//  vector_push(&v, 9);
//  printf("%d\n", *j); // invalid j is 9 now;
// ```
void* vector_pop(struct vector*);

// debug
void vector_print(struct vector*, const char*);

#endif

