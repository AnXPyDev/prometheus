void *StandardAllocator_malloc(void *this, Size size) {
    return malloc(size);
}

void *StandardAllocator_calloc(void *this, Size size) {
    return calloc(size, 1);
}

void *StandardAllocator_realloc(void *this, void *mem, Size size) {
    return realloc(mem, size);
}

void StandardAllocator_free(void *this, void *mem) {
    free(mem);
}

void StandardAllocator_destroy(void *this) {}

const IAllocator IStandardAllocator = {
    .malloc = &StandardAllocator_malloc,
    .calloc = &StandardAllocator_calloc,
    .realloc = &StandardAllocator_realloc,
    .free = &StandardAllocator_free,
    .destroy = &StandardAllocator_destroy
};

const Allocator g_standardAllocator = {
    .interface = &IStandardAllocator,
    .object = NULL
};
