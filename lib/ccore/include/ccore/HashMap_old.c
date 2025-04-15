#ifndef CCORE_HASHMAP_BUCKETS
#define CCORE_HASHMAP_BUCKETS 16
#endif

#ifndef CCORE_HASHMAP_ITEM_ALIGNMENT
#define CCORE_HASHMAP_ITEM_ALIGNMENT 8
#endif

#define MAP_BUCKETS CCORE_HASHMAP_BUCKETS

typedef uint32_t HashMap_Hash;
typedef uint32_t HashMap_EntryIndex;

#define HASHMAP_ENTRY_INDEX_MAX UINT32_MAX

typedef struct HashMap_Entry {
    struct HashMap_Entry *next;
    HashMap_EntryIndex index;
    HashMap_Hash hash;
    Size key_size;
    char tail[];
} HashMap_Entry;

#define ENTRY_ITEM(entry) ((entry)->tail)
#define ENTRY_KEY(entry, item_size) (ENTRY_ITEM(entry) + item_size)
#define ITEM_ENTRY(item) ((HashMap_Entry*)((char*)(item) - offsetof(HashMap_Entry, tail)))

typedef struct {
    Size item_size;
    HashMap_EntryIndex counter;
    Size nitems;
    HashMap_Entry *buckets[MAP_BUCKETS];
} HashMap;

typedef BufferView HashMap_Key;

void HashMap_create(HashMap *this, Size item_size) {
    this->item_size = memalign(item_size, CCORE_HASHMAP_ITEM_ALIGNMENT);
    this->counter = 0;
    this->nitems = 0;
    for (Size i = 0; i < MAP_BUCKETS; i++) {
        this->buckets[i] = NULL;
    }
}

HashMap_Entry *HashMap_createEntry(HashMap *this, HashMap_Hash hash, HashMap_Key key, Allocator alc) {
    HashMap_Entry *entry = Allocator_calloc(alc, sizeof(struct HashMap_Entry) + this->item_size + key.size);
    entry->key_size = key.size;
    entry->hash = hash;
    entry->index = this->counter;
    this->counter += 1;
    memcpy(ENTRY_KEY(entry, this->item_size), key.data, key.size);
    return entry;
}

void HashMap_destroyEntry(HashMap_Entry *entry, Allocator alc) {
    Allocator_free(alc, entry);
}

HashMap_Hash HashMap_hash(HashMap_Key key) {
    Size size = key.size;
    const char *data = key.data;

    HashMap_Hash hash = 5381;
	while ( size != 0 ) {
        HashMap_Hash val = (HashMap_Hash)*data;
        hash = ((hash << 5) + hash) + val;
        data++;
        size--;
	}
    return hash;
}

#define MAP_BUCKET(hash) (hash % MAP_BUCKETS)

HashMap_Entry *HashMap_pushKey(HashMap *this, HashMap_Hash hash, HashMap_Key key, Allocator alc) {
    HashMap_Entry *entry = HashMap_createEntry(this, hash, key, alc);
    entry->next = this->buckets[MAP_BUCKET(hash)];
    this->buckets[MAP_BUCKET(hash)] = entry;
    this->nitems++;
    return entry;
}

void HashMap_removeEntry(HashMap *this, HashMap_Entry **entryp, Allocator alc) {
    HashMap_Entry *next = (*entryp)->next;
    HashMap_destroyEntry(*entryp, alc);
    *entryp = next;
    this->nitems--;
}

HashMap_Entry *HashMap_getBucket(HashMap *this, HashMap_Hash hash) {
    return this->buckets[MAP_BUCKET(hash)];
}

HashMap_Entry *HashMap_findEntryInBucket(HashMap *this, HashMap_Entry *bucket, HashMap_Hash hash, HashMap_Key key) {
    HashMap_Entry *entry = bucket; 
    while (1) {
        if (entry == NULL) { return NULL; }
        if (entry->hash == hash && entry->key_size == key.size && memcmp(ENTRY_KEY(entry, this->item_size), key.data, key.size) == 0) {
            return entry;
        }
        entry = entry->next;
    }
}

HashMap_Entry *HashMap_findEntry(HashMap *this, HashMap_Hash hash, HashMap_Key key) {
    return HashMap_findEntryInBucket(this, HashMap_getBucket(this, hash), hash, key);
}

HashMap_Entry **HashMap_findEntryP(HashMap *this, HashMap_Hash hash, HashMap_Key key) {
    HashMap_Entry **entryp = this->buckets + MAP_BUCKET(hash);
    while (1) {
        HashMap_Entry *entry = *entryp;
        if (entry == NULL) { return NULL; }
        if (entry->hash == hash && entry->key_size == key.size && memcmp(ENTRY_KEY(entry, this->item_size), key.data, key.size) == 0) {
            return entryp;
        }
        entryp = &entry->next;
    }
}

void HashMap_destroyBucket(HashMap_Entry *entry, Allocator alc) {
    HashMap_Entry *next;
    while (1) {
        if (entry == NULL) { return; }
        next = entry->next;
        HashMap_destroyEntry(entry, alc);
        entry = next;
    }
}

void *HashMap_add(HashMap *this, HashMap_Key key, Allocator alc) {
    HashMap_Hash hash = HashMap_hash(key);
    HashMap_Entry *entry = HashMap_pushKey(this, hash, key, alc);
    return ENTRY_ITEM(entry);
}

void *HashMap_probe_ensure(HashMap *this, HashMap_Key key, bool *had_item, Allocator alc) {
    HashMap_Hash hash = HashMap_hash(key);
    HashMap_Entry *entry = HashMap_findEntry(this, hash, key);
    if (entry == NULL) {
        *had_item = false;
        entry = HashMap_pushKey(this, hash, key, alc);
    } else {
        *had_item = true;
    }
    return ENTRY_ITEM(entry);
}

void *HashMap_ensure(HashMap *this, HashMap_Key key, Allocator alc) {
    bool had;
    return HashMap_probe_ensure(this, key, &had, alc);
}

void *HashMap_get(HashMap *this, HashMap_Key key) {
    HashMap_Hash hash = HashMap_hash(key);
    HashMap_Entry *entry = HashMap_findEntry(this, hash, key);
    if (entry == NULL) {
        return NULL;
    }
    return ENTRY_ITEM(entry);
}

void HashMap_delete(HashMap *this, HashMap_Key key, Allocator alc) {
    HashMap_Hash hash = HashMap_hash(key);
    HashMap_Entry **entryp = HashMap_findEntryP(this, hash, key);
    if (entryp == NULL) {
        return;
    }
    HashMap_removeEntry(this, entryp, alc);
}

void HashMap_destroy(HashMap *this, Allocator alc) {
    for (Size i = 0; i < MAP_BUCKETS; i++) {
        HashMap_destroyBucket(this->buckets[i], alc);
        this->buckets[i] = NULL;
    }
}

BufferView HashMap_getKey(HashMap *this, void *item) {
    HashMap_Entry *entry = ITEM_ENTRY(item);
    return (BufferView) { .data = ENTRY_KEY(entry, this->item_size), .size = entry->key_size };
}

HashMap_EntryIndex HashMap_getIndex(void *item) {
    HashMap_Entry *entry = ITEM_ENTRY(item);
    return entry->index;
}

Array HashMap_matching(HashMap *this, HashMap_Key key, Allocator alc) {
    HashMap_Hash hash = HashMap_hash(key);

    Vector items;
    Vector_create(&items, sizeof(void*));
    Vector_init(&items, 8, alc);

    HashMap_Entry *entry = HashMap_getBucket(this, hash);
    while (true) {
        entry = HashMap_findEntryInBucket(this, entry, hash, key);
        if (entry) *(void**)Vector_push(&items, alc) = ENTRY_ITEM(entry);
        else break;
    }

    return Vector_array(&items);
}

Array HashMap_ordered_matching(HashMap *this, HashMap_Key key, Allocator alc) {
    Array arr = HashMap_matching(this, key, alc);

    void **front = arr.data;
    void **end = front + arr.size - 1;
    for (Size i = 0; i < arr.size / 2; i++) {
        void *temp = *front;
        *(front++) = *end;
        *(end--) = temp;
    }

    return arr;
}

Array HashMap_items(HashMap *this, Allocator alc) {
    void **items = Allocator_malloc(alc, sizeof(void*) * this->nitems);
    void **ip = items;
    
    for (Size i = 0; i < MAP_BUCKETS; i++) {
        HashMap_Entry *entry = this->buckets[i];
        while (entry) {
            *(ip++) = ENTRY_ITEM(entry);
            entry = entry->next; 
        }
    }

    return (Array) { .data = items, .size = this->nitems };
}

Array HashMap_ordered_items(HashMap *this, Allocator alc, Allocator temp_alc) {
    const Size nitems = this->nitems;

    char *buf = Allocator_malloc(temp_alc, (sizeof(void*) + sizeof(HashMap_EntryIndex)) * nitems);
    void **items = (void**)buf;
    HashMap_EntryIndex *indices = (HashMap_EntryIndex*)(buf + sizeof(void*) * nitems);
    void **it = items;
    HashMap_EntryIndex *ix = indices;

    for (Size i = 0; i < MAP_BUCKETS; i++) {
        HashMap_Entry *entry = this->buckets[i];
        while (entry) {
            *(it++) = ENTRY_ITEM(entry);
            *(ix++) = entry->index;
            entry = entry->next; 
        }
    }

    void **sorted = Allocator_malloc(alc, sizeof(void*) * nitems);
    void **sp = sorted;

    HashMap_EntryIndex *ix_end = indices + nitems; 
    for (ix = indices; ix < ix_end - 1; ix++) {
        HashMap_EntryIndex *minp = ix;
        HashMap_EntryIndex min = *ix;
        for (HashMap_EntryIndex *ix2 = ix + 1; ix2 < ix_end; ix2++) {
            HashMap_EntryIndex val = *ix2;
            if (val < min) {
                min = val;
                minp = ix2;
            }
        }
        *(sp++) = items[minp - indices];
    }

    *sp = items[nitems - 1];

    Allocator_free(temp_alc, buf);

    return (Array) { .data = sorted, .size = nitems };
}

#undef ENTRY_ITEM
#undef ENTRY_KEY
#undef ITEM_ENTRY
#undef MAP_BUCKET
#undef MAP_BUCKETS
