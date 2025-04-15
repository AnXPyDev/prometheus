typedef struct {
    void *(*malloc )(void *this, Size size);
    void *(*calloc )(void *this, Size size);
    void *(*realloc)(void *this, void *mem, Size size);
    void  (*free   )(void *this, void *mem);
    void  (*destroy)(void *this);
} IAllocator;

typedef struct {
    const IAllocator *interface;
    void *object;
} Allocator;

const Allocator Allocator_NULL = {
    .interface = NULL,
    .object = NULL
};

bool Allocator_isNull(Allocator this) {
    return this.interface == NULL;
}

void *Allocator_malloc(Allocator this, Size size) {
    return this.interface->malloc(this.object, size);
}

void *Allocator_calloc(Allocator this, Size size) {
    return this.interface->calloc(this.object, size);
}

void *Allocator_realloc(Allocator this, void *mem, Size size) {
    return this.interface->realloc(this.object, mem, size);
}

void Allocator_free(Allocator this, void *mem) {
    this.interface->free(this.object, mem);
}

void Allocator_destroy(Allocator this) {
    this.interface->destroy(this.object);
}

#ifdef LIB_DISABLE_ALLOCATOR
#define Allocator_malloc(alc, size) malloc(size)
#define Allocator_calloc(alc, size) calloc(size, 1)
#define Allocator_realloc(alc, buf, size) realloc(buf, size)
#define Allocator_free(alc, buf) free(buf)
#endif

#define Allocator_new(allocator, T) ((T*)Allocator_calloc((allocator), sizeof(T)))
