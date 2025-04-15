typedef struct HashMap_Entry {
	struct HashMap_Entry *next;
	Size index;
	Hash hash;
	char object[];
} HashMap_Entry;

HashMap_Entry *HashMap_entryFromObject(void *object) {
	return (HashMap_Entry*)((char*)(object) - offsetof(HashMap_Entry, object));
}

typedef struct {
	Size index;
	Size size;
	Size buckets_size;
	HashMap_Entry *buckets[];
} HashMap;

typedef bool (*HashMap_match_fn_t)(void *object, void *key);

typedef struct {
	HashMap_match_fn_t match;
	void *payload;
} HashMap_Key;

HashMap *HashMap_create(Size buckets, Allocator alc) {
	HashMap *this = Allocator_calloc(alc, sizeof(HashMap) + sizeof(HashMap_Entry) * buckets);
	this->buckets_size = buckets;
	this->size = 0;
	this->index = 0;
	return this;
}

void HashMap_destroyBucket(HashMap_Entry *entry, Allocator alc) {
	if (!entry) return;
	HashMap_destroyBucket(entry->next, alc);
	Allocator_free(alc, entry);
}

void HashMap_clear(HashMap *this, Allocator alc) {
	HashMap_Entry **it = this->buckets;
	HashMap_Entry **end = it + this->buckets_size;
	for (; it < end; it++) {
		HashMap_destroyBucket(*it, alc);
		*it = NULL;
	}
}

void HashMap_destroy(HashMap *this, Allocator alc) {
	HashMap_clear(this, alc);
	Allocator_free(alc, this);
}

HashMap_Entry **HashMap_getBucket(HashMap *this, Hash hash) {
	return &this->buckets[hash % this->buckets_size];
}

HashMap_Entry *HashMap_nextHash(HashMap_Entry *entry, Hash hash) {
	while (entry) {
		if (entry->hash == hash) return entry;	
		entry = entry->next;
	}
	return NULL;
}

HashMap_Entry **HashMap_findEntryOwner(HashMap *this, HashMap_Entry *entry) {
	HashMap_Entry **entryp = HashMap_getBucket(this, entry->hash);

	while (entryp) {
		if (*entryp == entry) return entryp;
		entryp = &(*entryp)->next;
	}

	return NULL;
}

HashMap_Entry *HashMap_addEntry(HashMap *this, Hash hash, Size size, Allocator alc) {
	HashMap_Entry **bucket = HashMap_getBucket(this, hash);
	HashMap_Entry *entry = Allocator_calloc(alc, sizeof(HashMap_Entry) + size);
	entry->hash = hash;
	entry->next = *bucket;
	entry->index = this->index++;
	*bucket = entry;
	this->size++;
	return entry;
}

void HashMap_removeEntry(HashMap *this, HashMap_Entry *entry, Allocator alc) {
	HashMap_Entry **owner = HashMap_findEntryOwner(this, entry);
	*owner = entry->next;
	this->size--;
	Allocator_free(alc, entry);
}

HashMap_Entry *HashMap_getNextEntry(
	HashMap_Entry *entry, HashMap_Key key
) {
	while (entry) {
		if (key.match(entry->object, key.payload)) return entry;
		entry = entry->next;
	}
	
	return NULL;
}

HashMap_Entry *HashMap_getEntry(HashMap *this, Hash hash, HashMap_Key key) {
	return HashMap_getNextEntry(
		*HashMap_getBucket(this, hash), key
	);
}

void *HashMap_get(HashMap *this, Hash hash, HashMap_Key key) {
	HashMap_Entry *entry = HashMap_getEntry(this, hash, key);
	if (!entry) return NULL;
	return entry->object;
}

void *HashMap_next(void *object, HashMap_Key key) {
	HashMap_Entry *entry = HashMap_entryFromObject(object);
	HashMap_Entry *next = HashMap_getNextEntry(entry->next, key);
	if (!next) return NULL;
	return next->object;
}

void *HashMap_ensure_probe(
	HashMap *this, Hash hash, Size size,
	HashMap_Key key, bool *out_existed, Allocator alc
) {
	HashMap_Entry *entry = HashMap_getEntry(this, hash, key);
	if (!entry) {
		*out_existed = false;
		entry = HashMap_addEntry(this, hash, size, alc);
	} else {
		*out_existed = true;
	}
	return entry->object;
}

void *HashMap_ensure(HashMap *this, Hash hash, Size size, HashMap_Key key, Allocator alc) {
	bool existed;
	return HashMap_ensure_probe(this, hash, size, key, &existed, alc);
}

void *HashMap_add(HashMap *this, Hash hash, Size size, Allocator alc) {
	HashMap_Entry *entry = HashMap_addEntry(this, hash, size, alc);
	return entry->object;
}

void HashMap_remove(HashMap *this, void *object, Allocator alc) {
	HashMap_Entry *entry = HashMap_entryFromObject(object);
	HashMap_removeEntry(this, entry, alc);
}

void HashMap_items(HashMap *this, void **out_items) {
	void **op = out_items;

	HashMap_Entry **it = this->buckets;
	HashMap_Entry **end = it + this->buckets_size;
	for (; it < end; it++) {
		HashMap_Entry *entry = *it;
		while (entry) {
			*(op++) = entry->object;
			entry = entry->next;
		}
	}
}

void HashMap_ordered_items(HashMap *this, void **out_items, Allocator tmp_alc) {
	HashMap_Entry **buckets = Allocator_malloc(tmp_alc, sizeof(HashMap_Entry*) * this->buckets_size);
	memcpy(buckets, this->buckets, sizeof(HashMap_Entry*) * this->buckets_size);

	void **op = out_items;

	HashMap_Entry **end = buckets + this->buckets_size;
	while (true) {
		Size max_index = 0;
		HashMap_Entry **max_bucket = NULL;

		HashMap_Entry **it = buckets;
		for (; it < end; it++) {
			HashMap_Entry *entry = *it;
			if (entry && entry->index >= max_index) {
				max_bucket = it;
				max_index = entry->index;
			}
		}
		if (!max_bucket) break;

		HashMap_Entry *max_entry = *max_bucket;
		*(op++) = max_entry->object;

		*max_bucket = max_entry->next;
	}

	op = out_items;
	void **ep = out_items + (this->size - 1);

	for (Size i = 0; i < this->size / 2; i++) {
		void *tmp = *op;
		*(op++) = *ep;
		*(ep--) = tmp;
	}

	Allocator_free(tmp_alc, buckets);
}

void HashMap_matching(HashMap *this, Hash hash, HashMap_Key key, Vector_Alc out) {
	HashMap_Entry *entry = *HashMap_getBucket(this, hash);

	while ((entry = HashMap_getNextEntry(entry, key))) {
		*(void**)Vector_push(out.vec, out.alc) = entry->object;
		entry = entry->next;
	}
}
