#include "mongoose/mongoose.h"
#include "registry.h"

static const char *s_http_addr = "http://0.0.0.0:9243";
static BangRegistry *registry;

static void fn(struct mg_connection *c, const int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = ev_data;
        if (mg_match(hm->uri, mg_str("/query/name"), NULL)) {
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

        const struct mg_http_serve_opts opts = {.root_dir = "public"};
        mg_http_serve_dir(c, hm, &opts);
    }
}

int main(void) {
    registry = registry_create(5);
    registry_register(registry, "g", "Google", "https://www.google.com/search?q=%%s");
    registry_register(registry, "r", "Reddit", "https://www.reddit.com/search/?q=%%s");

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, s_http_addr, fn, NULL);
    printf("Backend server started on %s\n", s_http_addr);

    for (;;) mg_mgr_poll(&mgr, 1000);

    mg_mgr_free(&mgr);
    registry_destroy(registry);
    return 0;
}
