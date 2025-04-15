void *StaticAllocator_malloc(void *this, Size size) {
    fprintf(stderr, "\nStaticAllocator: attempt to malloc with static allocator\n");
    fflush(stderr);
    abort();

    return NULL;
}

void *StaticAllocator_calloc(void *this, Size size) {
    fprintf(stderr, "\nStaticAllocator: attempt to calloc with static allocator\n");
    fflush(stderr);
    abort();

    return NULL;
}

void *StaticAllocator_realloc(void *this, void *mem, Size size) {
    fprintf(stderr, "\nStaticAllocator: attempt to calloc with static allocator\n");
    fflush(stderr);
    abort();

    return NULL;
}

void StaticAllocator_free(void *this, void *mem) {
    return;
}

void StaticAllocator_destroy(void *this) {}

const IAllocator IStaticAllocator = {
    .malloc = &StaticAllocator_malloc,
    .calloc = &StaticAllocator_calloc,
    .realloc = &StaticAllocator_realloc,
    .free = &StaticAllocator_free,
    .destroy = &StaticAllocator_destroy
};

const Allocator g_staticAllocator = {
    .interface = &IStaticAllocator,
    .object = NULL
};
