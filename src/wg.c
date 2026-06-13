#include "wg.h"
#include "buff.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFSZ 1024

int wg_get_peer_stats(struct vector *peers, const char *interface)
{
  FILE *f;
  char buff[BUFFSZ];
  size_t nread;
  struct buff out;

  buff_init(&out);

  char cmd[256];
  snprintf(cmd, 256, "wg show %s dump", interface);

  f = popen(cmd, "r");
  if (!f)
  {
    perror("popen()");
    return -1;
  }

  while (1)
  {
    nread = fread(buff, 1, BUFFSZ, f);
    if (nread < 1)
    {
      if (ferror(f))
      {
        perror("ferror()");
        return -1;
      }
      if (feof(f))
        break;
    }
    if (nread < BUFFSZ)
      buff_push(&out, buff, nread+1); // last push, we make space for null termination
    else
      buff_push(&out, buff, nread);
  }
  buff_data(&out)[out.len-1] = 0;

  int ret = pclose(f);
  if (ret)
  {
    printf("cmd failed: %s\n", cmd);
    return -1;
  }

  char *data = buff_data(&out);
  char *sl = strchr(data, '\n');
  if (!sl)
  {
    fprintf(stderr, "could not find second line\n");
    return -1;
  }

  sl++;
  char *start = sl;
  char *next;
  struct peer_stats stats;
begin:
  next = strtok(start, "\t");
  if (!next) goto end;
  start = NULL;
  strncpy(stats.pubkey, next, sizeof(stats.pubkey));
  strtok(NULL, "\t");
  strtok(NULL, "\t");
  next = strtok(NULL, "\t");
  strncpy(stats.ip, next, sizeof(stats.ip));
  strtok(NULL, "\t");
  next = strtok(NULL, "\t");
  errno = 0;
  stats.rx_bytes = strtoull(next, NULL, 10);
  if (errno)
  {
    perror("strtoull()");
    return -1;
  }
  next = strtok(NULL, "\t");
  errno = 0;
  stats.tx_bytes = strtoull(next, NULL, 10);
  if (errno)
  {
    perror("strtoull()");
    return -1;
  }
  strtok(NULL, "\n");
  vector_push(peers, &stats);
  goto begin;

end:
  buff_cleanup(&out);
  return 0;
}

void peer_stats_print(struct peer_stats *stats)
{
  printf("%s\t%s\t%llu\t%llu\n", stats->pubkey, stats->ip, stats->rx_bytes, stats->tx_bytes);
}
