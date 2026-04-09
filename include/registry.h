#ifndef REGISTRY_H
#define REGISTRY_H

typedef struct {
    char *label;
    char *name;
    char *url;
} BangEntry;

typedef struct {
    BangEntry **entries;
    int size;
    int capacity;
} BangRegistry;

BangRegistry *registry_create(int initial_capacity);

void registry_grow(BangRegistry *registry);

void registry_register(BangRegistry *registry, const char *label, const char *name, const char *url);

void registry_destroy(const BangRegistry *registry);

const char *registry_lookup_name(const BangRegistry *registry, const char *label);

const char *registry_lookup_url(const BangRegistry *registry, const char *label);

#endif // REGISTRY_H
