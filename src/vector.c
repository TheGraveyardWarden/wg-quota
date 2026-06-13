#include "vector.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

#define __vector_check_idx(v, idx) do {\
    if (idx > v->len-1) { \
      LOG_ERR("[vector_at] idx out of bound\n"); \
      exit(1); \
    } \
  } while (0)

struct vector __vector(unsigned int type_sz) {
  return __vector_with_capacity(type_sz, VEC_DEFAULT_CAP);
}

struct vector __vector_with_capacity(unsigned int type_sz, unsigned int cap) {
  struct vector v;

  if ((v.data = malloc(type_sz*cap)) == NULL) {
    LOG_ERR("[__vector_with_capacity] no mem\n");
    exit(1);
  }

  v.type_sz = type_sz;
  v.len = 0;
  v.cap = cap;

  return v;
}

struct vector vector_copy(struct vector *v)
{
  struct vector ret = __vector_with_capacity(v->type_sz, v->cap);
  memcpy(ret.data, v->data, v->cap * v->type_sz);
  ret.len = v->len;
  return ret;
}

struct vector vector_move(struct vector *v)
{
  struct vector moved = *v;
  bzero(v, sizeof(struct vector));
  return moved;
}

void vector_delete(struct vector *v)
{
  if (v->data)
    free(v->data);
  bzero(v, sizeof(struct vector));
}

void vector_delete_shared_ptr(void *v)
{
  vector_delete(v);
}

void vector_delete_shared_ptr_p(void *v)
{
	vector_delete(*(void**)v);
}

static inline void* __vector_at(struct vector *v, unsigned int idx)
{
  return ((char*)v->data) + (idx * v->type_sz);
}

void* vector_at(struct vector *v, unsigned int idx) {
  __vector_check_idx(v, idx);
  return __vector_at(v, idx);
}

void vector_reserve(struct vector *v, unsigned int additional) {
  if (v->data == NULL || additional == 0)
    return;

  unsigned int new_sz = (additional + v->cap) * v->type_sz;
  if ((v->data = realloc(v->data, new_sz)) == NULL) {
    LOG_ERR("[vector_reserve] no mem\n");
    exit(1);
  }
  v->cap += additional;
}

void vector_push(struct vector *v, void *new) {
  if (v->len == v->cap)
    vector_reserve(v, VEC_DEFAULT_CAP);

  memcpy(__vector_at(v, v->len++), new, v->type_sz);
}

void* vector_emplace(struct vector *v)
{
  if (v->len == v->cap)
    vector_reserve(v, VEC_DEFAULT_CAP);

  return __vector_at(v, v->len++);
}

void vector_remove(struct vector *v, unsigned int idx)
{
  __vector_check_idx(v, idx);
  char *item;

  for (item = __vector_at(v, idx+1); item < (char*)__vector_at(v, v->len); item += v->type_sz)
    memcpy(item - v->type_sz, item, v->type_sz);

  v->len--;
}

void vector_swap_remove(struct vector *v, unsigned int idx)
{
  __vector_check_idx(v, idx);
  if (idx == v->len-1)
  {
    v->len--;
    return;
  }

  memcpy(__vector_at(v, idx), __vector_at(v, --v->len), v->type_sz);
}

// experimental
void* vector_pop(struct vector *v)
{
  if (vector_is_empty(v)) return NULL;

  return __vector_at(v, --v->len);
}

void vector_print(struct vector *v, const char *debug)
{
  printf("vector %s { len: %d, cap: %d, type_sz: %d, data: %p }\n", debug, v->len, v->cap, v->type_sz, v->data);
}

