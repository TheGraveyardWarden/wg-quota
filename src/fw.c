#include "fw.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct fw_iptables
{
  struct fw_funcs func;
};

int fw_iptables_disable(struct fw_iptables *fw, const char *ip)
{
  if (!fork())
  {
    execl("/usr/sbin/iptables", "iptables", "-I", "FORWARD", "-s", ip, "-j", "DROP", NULL);
    execl("/usr/sbin/iptables", "iptables", "-I", "FORWARD", "-d", ip, "-j", "DROP", NULL);
  }
  return 0;
}

int fw_iptables_enable(struct fw_iptables *fw, const char *ip)
{
  if (!fork())
  {
    execl("/usr/sbin/iptables", "iptables", "-D", "FORWARD", "-s", ip, "-j", "DROP", NULL);
    execl("/usr/sbin/iptables", "iptables", "-D", "FORWARD", "-d", ip, "-j", "DROP", NULL);
  }
  return 0;
}

static struct fw* fw_iptables_init()
{
  struct fw_iptables *fw;

  fw = malloc(sizeof(struct fw_iptables));
  if (!fw)
  {
    fprintf(stderr, "no mem!\n");
    return NULL;
  }
  bzero(fw, sizeof(struct fw_iptables));

  fw->func.disable = fw_iptables_disable;
  fw->func.enable = fw_iptables_enable;

  return (struct fw*)fw;
}

struct fw* fw_init()
{
  return fw_iptables_init();
}

