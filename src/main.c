#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "db.h"
#include "wg.h"
#include "fw.h"

#define DB_FILE "./wg.db"
#define WGCTL_INTERFACE "wg0"
#define SLEEP_TIME 10

int main()
{
  struct db *db = db_init(DB_FILE);
  struct fw *fw = fw_init();
  struct vector peers = vector(struct peer_stats);
loop:
  int rc = wg_get_peer_stats(&peers, WGCTL_INTERFACE);
  if (rc)
  {
    fprintf(stderr, "wg_get_peer_stats(): error\n");
    return -1;
  }

  struct peer_stats *p;
  struct peer_info info;
  time_t now;
  vector_for_each(&peers, p)
  {
    bool exists = db_peer_exists(db, p->pubkey);
    if (!exists)
    {
      printf("peer %s doesnt exist, continue...\n", p->ip);
      continue;
    }
    rc = db_get_peer_info(db, p->pubkey, &info);
    if (rc < 0)
    {
      printf("idk wht happend in getting peer info\n");
      continue;
    }

    uint64_t _last_used_bytes = info.last_used_bytes, _used_bytes;
    uint64_t _total = p->rx_bytes + p->tx_bytes;
    int _blocked = info.blocked;
    if (_total < info.used_bytes)
      _last_used_bytes += info.used_bytes;
    _used_bytes = _total;

    if (info.deadline == 0)
      goto check_bytes;
    now = time(NULL);
    if (now > info.deadline)
    {
      if (!_blocked)
      {
        _blocked = 1;
      }
      goto decided;
    }
    else
    {
      if (_blocked)
      {
        _blocked = 0;
      }
    }

check_bytes:
    if (_used_bytes + _last_used_bytes > info.quota_bytes)
    {
      if (!_blocked)
      {
        _blocked = 1;
      }
    }
    else
    {
      if (_blocked)
      {
        _blocked = 0;
      }
    }

decided:
    printf("updating %s: used: %llu, last_used: %llu, blocked: %d\n", p->ip, _used_bytes, _last_used_bytes, _blocked);

    if (_blocked && !info.blocked)
      fw_disable(fw, p->ip);
    else if (!_blocked && info.blocked)
      fw_enable(fw, p->ip);

    rc = db_update_peer_info(db, p->pubkey, _used_bytes, _last_used_bytes, _blocked);
    if (rc < 0)
    {
      printf("idk wht happend in updating peer info\n");
      continue;
    }
  }
  peers.len = 0;
  sleep(SLEEP_TIME);
  goto loop;

  vector_delete(&peers);
  db_close(db);
  fw_delete(fw);
	return 0;
}

