#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <sqlite3.h>

typedef struct {
    sqlite3 *db;
} AppContext;

bool db_init(const char *path, AppContext *ctx);

void db_bootstrap(sqlite3 *db);

// Bangs

char *db_get_name(sqlite3 *db, const char *prefix);

char *db_get_url(sqlite3 *db, const char *prefix);

bool db_save_bang(sqlite3 *db, const char *prefix, const char *name, const char *url);

// Keys

bool db_verify_key(sqlite3 *db, const char *key);

bool db_save_key(sqlite3 *db, const char *key);

#endif // DATABASE_H
