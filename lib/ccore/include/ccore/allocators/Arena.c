#ifndef CCORE_ARENA_ALLOCATOR_ALIGNMENT
#define CCORE_ARENA_ALLOCATOR_ALIGNMENT 8
#endif 

#define MEMALIGN(x) memalign((x), CCORE_ARENA_ALLOCATOR_ALIGNMENT)

#define ARENA_ALLOCATOR_PREDICTED_HEADERS 32

typedef struct ArenaAllocator_Chunk {
    struct ArenaAllocator_Chunk *previous;
    Size size;
    Size offset;
} ArenaAllocator_Chunk;

typedef struct {
    Allocator provider;
    Size chunkSize;
    ArenaAllocator_Chunk *chunk;
} ArenaAllocator;

typedef struct {
    Size size;
} ArenaAllocator_BufHeader;

#define CHUNK_HEADER_SIZE MEMALIGN(sizeof(ArenaAllocator_Chunk))
#define HEADER_SIZE MEMALIGN(sizeof(ArenaAllocator_BufHeader))
#define CHUNK_DATA(c) ((char*)(c) + CHUNK_HEADER_SIZE)

int ArenaAllocator_push(ArenaAllocator *this, Size size) {
    Size chunkSize = MEMALIGN(size + HEADER_SIZE * ARENA_ALLOCATOR_PREDICTED_HEADERS);
    ArenaAllocator_Chunk *chunk = Allocator_malloc(this->provider, CHUNK_HEADER_SIZE + chunkSize);
    if (!chunk) {
        return 0;
    }
    chunk->previous = this->chunk;
    chunk->offset = 0;
    chunk->size = chunkSize;
    this->chunk = chunk;
    return 1;
    //fprintf(stderr, "push %zu\n", size);
}

void ArenaAllocator_create(ArenaAllocator *this, Allocator provider, Size chunkSize) {
    this->provider = provider;
    this->chunkSize = chunkSize;
    this->chunk = NULL;
    ArenaAllocator_push(this, this->chunkSize);
}

#define this ((ArenaAllocator*)vthis)

void *ArenaAllocator_malloc(void *vthis, Size size) {
    Size stripSize = MEMALIGN(size + HEADER_SIZE);

    if (this->chunk->offset + stripSize > this->chunk->size) {
        if (!ArenaAllocator_push(this, MAX2(size, this->chunkSize))) {
            return NULL;
        }
    }

    const Size offset = this->chunk->offset;
    ArenaAllocator_BufHeader *header = (ArenaAllocator_BufHeader*)(CHUNK_DATA(this->chunk) + offset);
    header->size = stripSize;
    this->chunk->offset += stripSize;

    return (char*)header + HEADER_SIZE;
}

void *ArenaAllocator_calloc(void *vthis, Size size) {
    void *buf = ArenaAllocator_malloc(vthis, size);
    if (!buf) {
        return NULL;
    }
    memset(buf, 0, size);
    return buf;
}

void ArenaAllocator_free(void *vthis, void *buf) {
    //fprintf(stderr, "free\n");
}

void *ArenaAllocator_realloc(void *vthis, void *buf, Size size) {
    //fprintf(stderr, "realloc\n");
    if (!buf) {
        return ArenaAllocator_malloc(this, size);
    }

    Size bsize = MEMALIGN(size + HEADER_SIZE);
    ArenaAllocator_BufHeader *header = (ArenaAllocator_BufHeader*)((char*)buf - HEADER_SIZE);
    if (bsize <= header->size) {
        return buf;
    }

    void *newBuf = ArenaAllocator_malloc(this, size);
    if (!newBuf) {
        return NULL;
    }
    memcpy(newBuf, buf, MIN2(header->size, size));
    return newBuf;
}

void ArenaAllocator_destroy(void *vthis) {
    ArenaAllocator_Chunk *chunk = this->chunk;
    while (chunk) {
        ArenaAllocator_Chunk *prev = chunk->previous;
        Allocator_free(this->provider, chunk);
        chunk = prev;
    }
}

#undef this

const IAllocator IArenaAllocator = {
    .free = &ArenaAllocator_free,
    .malloc = &ArenaAllocator_malloc,
    .calloc = &ArenaAllocator_calloc,
    .realloc = &ArenaAllocator_realloc,
    .destroy = &ArenaAllocator_destroy
};

Allocator ArenaAllocator_upcast(ArenaAllocator *this) {
    return (Allocator) {
        .interface = &IArenaAllocator,
        .object = this
    };
}

#undef HEADER_SIZE
#undef CHUNK_HEADER_SIZE
#undef CHUNK_DATA
#undef ARENA_ALLOCATOR_PREDICTED_HEADERS
#undef MEMALIGN
