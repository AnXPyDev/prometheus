#ifndef CCORE_MONITOR_ALLOCATOR_ALIGNMENT
#define CCORE_MONITOR_ALLOCATOR_ALIGNMENT 8
#endif 

#define MEMALIGN(x) memalign((x), CCORE_MONITOR_ALLOCATOR_ALIGNMENT)

typedef struct {
    Allocator provider;
    Size malloc_total;
    Size malloc_count;
    HPTimer_Duration malloc_timer;
    Size calloc_total;
    Size calloc_count;
    HPTimer_Duration calloc_timer;
    Size realloc_total;
    Size realloc_count;
    HPTimer_Duration realloc_timer;
    Size free_total;
    Size free_count;
    HPTimer_Duration free_timer;
    HPTimer_Duration destroy_timer;
    OutStream logStream;
} MonitorAllocator;

typedef struct {
    Size size;
} MonitorAllocator_BufHeader;

#define HEADER_SIZE MEMALIGN(sizeof(MonitorAllocator_BufHeader))

void MonitorAllocator_create(MonitorAllocator *this, Allocator provider, OutStream logStream) {
    this->provider = provider;
    this->logStream = logStream;
    this->malloc_total = 0;
    this->malloc_count = 0;
    this->malloc_timer = 0;
    this->realloc_total = 0;
    this->realloc_count = 0;
    this->realloc_timer = 0;
    this->free_total = 0;
    this->free_count = 0;
    this->free_timer = 0;
    this->destroy_timer = 0;
}

#define this ((MonitorAllocator*)vthis)

void *MonitorAllocator_malloc(void *vthis, Size size) {
    HPTimer_Measurement timer_start = HPTimer_now();
    MonitorAllocator_BufHeader *header = Allocator_malloc(this->provider, size + HEADER_SIZE);
    HPTimer_Duration timer = HPTimer_elapsed(timer_start);
    header->size = size;
    void *result = (char*)header + HEADER_SIZE;

    this->malloc_timer += timer;
    this->malloc_count++;
    this->malloc_total += size;

    if (!OutStream_isNull(this->logStream)) {
        char msg[256];
        sprintf(msg, "MonitorAllocator(%p): malloc  %p (%zuB) T=%lfms\n", (void*)this, result, size, HPTimer_millis(timer));
        OutStream_puts(this->logStream, msg);
    }

    return result;
}

void *MonitorAllocator_calloc(void *vthis, Size size) {
    HPTimer_Measurement timer_start = HPTimer_now();
    MonitorAllocator_BufHeader *header = Allocator_calloc(this->provider, size + HEADER_SIZE);
    HPTimer_Duration timer = HPTimer_elapsed(timer_start);
    header->size = size;
    void *result = (char*)header + HEADER_SIZE;

    this->calloc_timer += timer;
    this->calloc_count++;
    this->calloc_total += size;

    if (!OutStream_isNull(this->logStream)) {
        char msg[256];
        sprintf(msg, "MonitorAllocator(%p): calloc  %p (%zuB) T=%lfms\n", (void*)this, result, size, HPTimer_millis(timer));
        OutStream_puts(this->logStream, msg);
    }

    return result;
}

void *MonitorAllocator_realloc(void *vthis, void *buf, Size size) {
    MonitorAllocator_BufHeader *oldHeader = (MonitorAllocator_BufHeader*)((char*)buf - HEADER_SIZE);
    Size oldSize = oldHeader->size;
    HPTimer_Measurement timer_start = HPTimer_now();
    MonitorAllocator_BufHeader *header = Allocator_realloc(this->provider, oldHeader, size + HEADER_SIZE);
    HPTimer_Duration timer = HPTimer_elapsed(timer_start);
    header->size = size;
    void *result = (char*)header + HEADER_SIZE;

    this->realloc_timer += timer;
    this->realloc_count++;
    this->realloc_total += size;

    if (!OutStream_isNull(this->logStream)) {
        char msg[256];
        sprintf(msg, "MonitorAllocator(%p): realloc %p => %p (%zuB => %zuB) T=%lfms\n", (void*)this, buf, result, oldSize, size, HPTimer_millis(timer));
        OutStream_puts(this->logStream, msg);
    }

    return result;
}

void MonitorAllocator_free(void *vthis, void *buf) {
    MonitorAllocator_BufHeader *header = (MonitorAllocator_BufHeader*)((char*)buf - HEADER_SIZE);
    Size size = header->size;
    HPTimer_Measurement timer_start = HPTimer_now();
    Allocator_free(this->provider, header);
    HPTimer_Duration timer = HPTimer_elapsed(timer_start);

    this->free_timer += timer;
    this->free_count++;
    this->free_total += size;

    if (!OutStream_isNull(this->logStream)) {
        char msg[256];
        sprintf(msg, "MonitorAllocator(%p): free    %p (%zuB) T=%lfms\n", (void*)this, buf, size, HPTimer_millis(timer));
        OutStream_puts(this->logStream, msg);
    }
}

void MonitorAllocator_destroy(void *vthis) {
    HPTimer_Measurement timer_start = HPTimer_now();
    Allocator_destroy(this->provider);
    HPTimer_Duration timer = HPTimer_elapsed(timer_start);
    this->destroy_timer = timer;

    if (!OutStream_isNull(this->logStream)) {
        char msg[256];
        sprintf(msg, "MonitorAllocator(%p): destroy T=%lfms\n", (void*)this, HPTimer_millis(timer));
        OutStream_puts(this->logStream, msg);
    }
}

#undef this

void MonitorAllocator_report(MonitorAllocator *this, OutStream stream) {
    char buf[256];
    snprintf(buf, 256, "MonitorAllocator(%p) report:\n", (void*)this);
    OutStream_puts(stream, buf);
    snprintf(buf, 256, "malloc  %zu (%zuB) T=%lfms\n", this->malloc_count, this->malloc_total, HPTimer_millis(this->malloc_timer));
    OutStream_puts(stream, buf);
    snprintf(buf, 256, "calloc  %zu (%zuB) T=%lfms\n", this->malloc_count, this->malloc_total, HPTimer_millis(this->malloc_timer));
    OutStream_puts(stream, buf);
    snprintf(buf, 256, "realloc %zu (%zuB) T=%lfms\n", this->realloc_count, this->realloc_total, HPTimer_millis(this->realloc_timer));
    OutStream_puts(stream, buf);
    snprintf(buf, 256, "free    %zu (%zuB) T=%lfms\n", this->free_count, this->free_total, HPTimer_millis(this->free_timer));
    OutStream_puts(stream, buf);
    snprintf(buf, 256, "destroy T=%lfms\n", HPTimer_millis(this->destroy_timer));
    OutStream_puts(stream, buf);
}

const IAllocator IMonitorAllocator = {
    .malloc = &MonitorAllocator_malloc,
    .calloc = &MonitorAllocator_calloc,
    .realloc = &MonitorAllocator_realloc,
    .free = &MonitorAllocator_free,
    .destroy = &MonitorAllocator_destroy
};

Allocator MonitorAllocator_upcast(MonitorAllocator *this) {
    return (Allocator) {
        .interface = &IMonitorAllocator,
        .object = this
    };
}

#undef HEADER_SIZE
#undef MEMALIGN
