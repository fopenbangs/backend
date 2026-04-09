#include <stdlib.h>
#include <string.h>
#include "registry.h"

BangRegistry *registry_create(const int initial_capacity) {
    BangRegistry *registry = malloc(sizeof(BangRegistry));
    registry->entries = malloc(sizeof(BangEntry *) * initial_capacity);
    registry->size = 0;
    registry->capacity = initial_capacity;
    return registry;
}

void registry_grow(BangRegistry *registry) {
    const int new_capacity = registry->capacity == 0 ? 10 : registry->capacity * 2;
    BangEntry **new_entries = realloc(registry->entries, sizeof(BangEntry *) * new_capacity);

    if (new_entries) {
        registry->entries = new_entries;
        registry->capacity = new_capacity;
    }
}

void registry_register(BangRegistry *registry, const char *label, const char *name, const char *url) {
    if (registry->size >= registry->capacity) registry_grow(registry);
    BangEntry *entry = malloc(sizeof(BangEntry));
    entry->label = strdup(label);
    entry->name = strdup(name);
    entry->url = strdup(url);
    registry->entries[registry->size++] = entry;
}

void registry_destroy(const BangRegistry *registry) {
    free(registry->entries);
}

const char *registry_lookup_name(const BangRegistry *registry, const char *label) {
    for (int i = 0; i < registry->size; i++) {
        if (strcmp(registry->entries[i]->label, label) == 0) return registry->entries[i]->name;
    }

    return NULL;
}

const char *registry_lookup_url(const BangRegistry *registry, const char *label) {
    for (int i = 0; i < registry->size; i++) {
        if (strcmp(registry->entries[i]->label, label) == 0) return registry->entries[i]->url;
    }

    return NULL;
}
