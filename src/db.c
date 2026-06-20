#include "db.h"

#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct db_sql
{
  struct db_funcs func;
  sqlite3 *db;
};

static void db_sql_close(struct db_sql *sql)
{
  sqlite3_close(sql->db);
  free(sql);
}

bool db_sql_peer_exists(struct db_sql *sql, const char *pubkey)
{
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(sql->db, "SELECT 1 FROM peers WHERE pubkey = ?;", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, pubkey, -1, SQLITE_STATIC);
  int rc = sqlite3_step(stmt);
  bool exists = rc == SQLITE_ROW;
  sqlite3_finalize(stmt);
  return exists;
}

int db_sql_get_peer_info(struct db_sql *sql, const char *pubkey, struct peer_info *out)
{
  int ret = -1;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(sql->db, "SELECT quota_bytes, used_bytes, last_used_bytes, deadline, blocked FROM peers WHERE pubkey = ?;", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, pubkey, -1, SQLITE_STATIC);
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW)
  {
    out->quota_bytes = (uint64_t)sqlite3_column_int64(stmt, 0);
    out->used_bytes = (uint64_t)sqlite3_column_int64(stmt, 1);
    out->last_used_bytes = (uint64_t)sqlite3_column_int64(stmt, 2);
    out->deadline = (int64_t)sqlite3_column_int64(stmt, 3);
    out->blocked = (int)sqlite3_column_int(stmt, 4);
    ret = 0;
  }
  sqlite3_finalize(stmt);
  return ret;
}

int db_sql_update_peer_info(struct db_sql *sql, const char *pubkey, uint64_t used_bytes, uint64_t last_used_bytes, int blocked)
{
  int ret = -1;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(sql->db, "UPDATE peers SET used_bytes = ?, last_used_bytes = ?, blocked = ? WHERE pubkey = ?;", -1, &stmt, NULL);
  sqlite3_bind_int64(stmt, 1, (sqlite3_int64)used_bytes);
  sqlite3_bind_int64(stmt, 2, (sqlite3_int64)last_used_bytes);
  sqlite3_bind_int(stmt, 3, blocked);
  sqlite3_bind_text(stmt, 4, pubkey, -1, SQLITE_STATIC);
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE)
    ret = 0;
  sqlite3_finalize(stmt);
  return ret;
}

static struct db* db_sql_init(const char *path)
{
  struct db_sql *sql;

  sql = malloc(sizeof(struct db_sql));
  if (!sql)
  {
    fprintf(stderr, "no mem!\n");
    return NULL;
  }
  bzero(sql, sizeof(struct db_sql));

  sql->func.close = db_sql_close;
  sql->func.peer_exists = db_sql_peer_exists;
  sql->func.get_peer_info = db_sql_get_peer_info;
  sql->func.update_peer_info = db_sql_update_peer_info;

  int rc = sqlite3_open(path, &sql->db);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(sql->db));
    sqlite3_close(sql->db);
    return NULL;
  }
  const char *sql_cmd =
    "CREATE TABLE IF NOT EXISTS peers ("
    "pubkey TEXT PRIMARY KEY,"
    "ip TEXT NOT NULL,"
    "quota_bytes INTEGER NOT NULL,"
    "used_bytes INTEGER NOT NULL,"
    "last_used_bytes INTEGER NOT NULL,"
    "deadline INTEGER NOT NULL DEFAULT 0,"
    "blocked INTEGER NOT NULL DEFAULT 0"
    ");";
  char *errmsg = NULL;
  rc = sqlite3_exec(sql->db, sql_cmd, NULL, NULL, &errmsg);
  if (rc != SQLITE_OK)
  {
      fprintf(stderr, "SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
  }

  return (struct db*)sql;
}

struct db* db_init(const char *path)
{
  return db_sql_init(path);
}

