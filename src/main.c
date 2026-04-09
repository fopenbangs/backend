#include "mongoose/mongoose.h"
#include "registry.h"
#include "database.h"

static const char *s_http_addr = "http://0.0.0.0:9243";
static BangRegistry *registry;

static void fn(struct mg_connection *c, const int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = ev_data;
        const AppContext *ctx = (AppContext *) c->fn_data;

        if (mg_match(hm->uri, mg_str("/query/name"), NULL)) {
            const struct mg_str *key = mg_http_get_header(hm, "X-API-Key");
            char key_buf[64] = {0};
            if (key) mg_snprintf(key_buf, sizeof(key_buf), "%.*s", (int) key->len, key->buf);
            if (!db_verify_key(ctx->db, key_buf)) {
                mg_http_reply(c, 401, "", "Unauthorized\n");
                return;
            }

            const struct mg_str bang_mg_str = mg_http_var(hm->query, mg_str("bang"));
            if (bang_mg_str.len == 0) {
                mg_http_reply(c, 404, "", "Bang not found!\n");
                return;
            }

            const char *bang_buf = bang_mg_str.buf;
            char label[16];
            sscanf(bang_buf, "%15s", label);

            const char *name = registry_lookup_name(registry, label);
            if (name) {
                mg_http_reply(c, 200, "", name);
                return;
            }

            mg_http_reply(c, 404, "", "Bang not found!\n");
            return;
        }

        if (mg_match(hm->uri, mg_str("/query/url"), NULL)) {
            const struct mg_str *key = mg_http_get_header(hm, "X-API-Key");
            char key_buf[64] = {0};
            if (key) mg_snprintf(key_buf, sizeof(key_buf), "%.*s", (int) key->len, key->buf);
            if (!db_verify_key(ctx->db, key_buf)) {
                mg_http_reply(c, 401, "", "Unauthorized\n");
                return;
            }

            const struct mg_str bang_mg_str = mg_http_var(hm->query, mg_str("bang"));
            if (bang_mg_str.len == 0) {
                mg_http_reply(c, 404, "", "Bang not found!\n");
                return;
            }

            const char *bang_buf = bang_mg_str.buf;
            char label[16];
            sscanf(bang_buf, "%15s", label);

            const char *url = registry_lookup_url(registry, label);
            if (url) {
                mg_http_reply(c, 302, "", url);
                return;
            }

            mg_http_reply(c, 404, "", "Bang not found!\n");
            return;
        }

        if (mg_match(hm->uri, mg_str("/register/bang"), NULL)) {
            char prefix[16], name[16], url[256];
            mg_http_get_var(&hm->body, "prefix", prefix, sizeof(prefix));
            mg_http_get_var(&hm->body, "name", name, sizeof(name));
            mg_http_get_var(&hm->body, "url", url, sizeof(url));
            if (db_save_bang(ctx->db, prefix, name, url)) mg_http_reply(c, 200, "", "Bang saved as !%s\n", prefix);
            else
                mg_http_reply(c, 500, "\r\n\r\nFailed to save bang!\n%s\n",
                              "You could be missing the (prefix,name,url) fields or our database broke.");
        }

        const struct mg_http_serve_opts opts = {.root_dir = "public"};
        mg_http_serve_dir(c, hm, &opts);
    }
}

int main(void) {
    AppContext ctx = {};
    if (!db_init("fopenbangs.db", &ctx)) {
        fprintf(stderr, "Failed to initialize database!\n");
        return 1;
    }

    registry = registry_create(5);
    db_bootstrap(ctx.db);

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, s_http_addr, fn, &ctx);
    printf("Backend server started on %s\n", s_http_addr);

    for (;;) mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
    registry_destroy(registry);
    return 0;
}
