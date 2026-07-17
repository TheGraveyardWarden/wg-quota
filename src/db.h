#ifndef _DB_H_
#define _DB_H_

#include <stdbool.h>
#include <stdio.h>

#include "peer.h"

struct peer_info
{
  uint64_t quota_bytes;
  uint64_t used_bytes;
  uint64_t last_used_bytes;
  int64_t deadline;
  int blocked;
};

static inline void peer_info_print(struct peer_info *info, const char *name)
{
  printf("peer_info %s { quota: %llu, used: %llu, last_used: %llu, deadline: %lld, blocked: %d }\n", name, info->quota_bytes, info->used_bytes, info->last_used_bytes, info->deadline, info->blocked);
}

struct db;

struct db_funcs
{
  void(*close)(struct db*);
  bool(*peer_exists)(struct db*, const char*);
  int(*get_peer_info)(struct db*, const char*, struct peer_info*);
  int(*update_peer_info)(struct db*, const char*, uint64_t, uint64_t, int);
};

struct db
{
  struct db_funcs func;
};

static inline void db_close(struct db *db)
{
  db->func.close(db);
}

static inline bool db_peer_exists(struct db *db, const char *pubkey)
{
  return db->func.peer_exists(db, pubkey);
}

static inline int db_get_peer_info(struct db *db, const char *pubkey, struct peer_info *out)
{
  return db->func.get_peer_info(db, pubkey, out);
}

static inline int db_update_peer_info(struct db *db, const char *pubkey, uint64_t used_bytes, uint64_t last_used_bytes, int blocked)
{
  return db->func.update_peer_info(db, pubkey, used_bytes, last_used_bytes, blocked);
}

struct db* db_init(const char *path);

#endif
