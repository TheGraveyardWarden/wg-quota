#ifndef _PEER_H_
#define _PEER_H_

#include <stdint.h>

struct peer_stats
{
  char pubkey[64];
  char ip[32];
  uint64_t rx_bytes;
  uint64_t tx_bytes;
};

#endif
