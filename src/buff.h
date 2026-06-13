#ifndef _BUFF_H_
#define _BUFF_H_

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "debug.h"

/*
 * simple buffer implementation
 * usage:
 * ```c
 * struct buff buff;
 * buff_init(&buff); // data: NULL, len: 0
 * buff_push(&buff, some_data, data_len); // data: heap_p, len: data_len
 * buff_push(&buff, some_data2, data_len2); // data: heap_p2, len: data_len + data_len2
 * buff_reset(&buff); // data: NULL, len: 0
 * // can reuse the buff, otherwise:
 * buff_cleanup(&buff); // data: NULL, len: 0
 * ```
 * */

struct buff
{
  unsigned int len;
  char *data;
};

#define buff_data(buffp) (buffp)->data

#define buff_init(buffp) (buffp)->len = 0; (buffp)->data = NULL;

int buff_push(struct buff *buff, const char *data, unsigned int len)
{
  CHECK_PTR(data, "[buff_push] data is NULL\n");
  CHECK_PTR(buff, "[buff_push] buff is NULL\n");
  if (len + buff->len < len)
  {
    fprintf(stderr, "[buff_push] int overflow\n");
    errno = EOVERFLOW;
    return -1;
  }

  if ((buff->data = realloc(buff->data, len + buff->len)) == NULL)
  {
    fprintf(stderr, "[buff_push] no mem!\n");
    errno = ENOMEM;
    return -1;
  }

  memcpy(buff->data + buff->len, data, len);
  buff->len += len;

  return 0;
}

int buff_cleanup(struct buff *buff)
{
  CHECK_PTR(buff, "[buff_cleanup] buff is NULL\n");
  free(buff->data);
  return 0;
}

int buff_reset(struct buff *buff)
{
  buff_cleanup(buff);
  buff_init(buff);
  return 0;
}

int buff_print(struct buff* buff, const char *name)
{
  CHECK_PTR(buff, "[buff_print] buff is NULL\n");
  printf("buff %s { data: %p, len: %ld }\n", name, buff->data, buff->len);
  return 0;
}

#endif
