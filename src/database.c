#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database.h"

bool db_init(const char *path, AppContext *ctx) {
    if (sqlite3_open(path, &ctx->db) != SQLITE_OK) return false;
    const char *sql =
            "CREATE TABLE IF NOT EXISTS bangs (prefix TEXT PRIMARY KEY, name TEXT NOT NULL, url TEXT NOT NULL);"
            "CREATE TABLE IF NOT EXISTS api_keys (key_value TEXT PRIMARY KEY);";

    return sqlite3_exec(ctx->db, sql, NULL, NULL, NULL) == SQLITE_OK;
}

void db_bootstrap(sqlite3 *db) {
    const char *sql =
            "PRAGMA journal_mode=WAL;"
            "CREATE TABLE IF NOT EXISTS bangs ("
            "  prefix TEXT PRIMARY KEY, "
            "  url TEXT NOT NULL"
            ");"
            "CREATE TABLE IF NOT EXISTS api_keys ("
            "  key_value TEXT PRIMARY KEY"
            ");";

    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "Startup error! %s\n", err);
        sqlite3_free(err);
    }
}

// Bangs

char *db_get_name(sqlite3 *db, const char *prefix) {
    sqlite3_stmt *stmt;
    char *url = NULL;
    if (sqlite3_prepare_v2(db, "SELECT name FROM bangs WHERE prefix = ?", -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, prefix, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) url = strdup((const char *) sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return url;
}

char *db_get_url(sqlite3 *db, const char *prefix) {
    sqlite3_stmt *stmt;
    char *url = NULL;
    if (sqlite3_prepare_v2(db, "SELECT url FROM bangs WHERE prefix = ?", -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, prefix, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) url = strdup((const char *) sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return url;
}

bool db_save_bang(sqlite3 *db, const char *prefix, const char *name, const char *url) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR REPLACE INTO bangs (prefix, name, url) VALUES (?, ?);";
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, prefix, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, url, -1, SQLITE_STATIC);
        success = sqlite3_step(stmt) == SQLITE_DONE;
    }

    sqlite3_finalize(stmt);
    return success;
}

// Keys

bool db_verify_key(sqlite3 *db, const char *key) {
    sqlite3_stmt *stmt;
    bool found = false;
    if (sqlite3_prepare_v2(db, "SELECT 1 FROM api_keys WHERE key_value = ?", -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
        found = sqlite3_step(stmt) == SQLITE_ROW;
    }

    sqlite3_finalize(stmt);
    return found;
}

bool db_save_key(sqlite3 *db, const char *key) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO api_keys (key_value) VALUES (?);";
    bool success = false;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
        success = sqlite3_step(stmt) == SQLITE_DONE;
    }

    sqlite3_finalize(stmt);
    return success;
}
