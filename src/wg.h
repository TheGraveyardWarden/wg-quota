#ifndef _WG_H_
#define _WG_H_

#include "vector.h"
#include "peer.h"

int wg_get_peer_stats(struct vector *peers, const char *interface);

void peer_stats_print(struct peer_stats *stats);

#endif
