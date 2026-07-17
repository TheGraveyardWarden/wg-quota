#ifndef _FW_H_
#define _FW_H_

#include <stdlib.h>

struct fw;

struct fw_funcs
{
  int(*disable)(struct fw*, const char*);
  int(*enable)(struct fw*, const char*);
};

struct fw
{
  struct fw_funcs func;
};

static inline int fw_disable(struct fw *fw, const char *ip)
{
  return fw->func.disable(fw, ip);
}

static inline int fw_enable(struct fw* fw, const char *ip)
{
  return fw->func.enable(fw, ip);
}

static inline void fw_delete(struct fw *fw)
{
  free(fw);
}

struct fw* fw_init();

#endif
