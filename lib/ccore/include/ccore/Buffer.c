typedef struct {
    char *data;
    Size size;
} Buffer;

typedef struct {
    const char *data;
    Size size;
} BufferView;

typedef Buffer StringBuffer;
typedef BufferView StringView;

const Buffer Buffer_NULL = {
    .data = NULL,
    .size = 0
};

const BufferView BufferView_NULL = {
    .data = NULL,
    .size = 0
};

StringView strview(const char *str) {
    return (StringView) {
        .data = str,
        .size = strlen(str)
    };
}

typedef struct {
    void *data;
    Size size;
} Array;

typedef struct {
    const void *data;
    Size size;
} ArrayView;

const Array Array_NULL = {
    .data = NULL,
    .size = 0
};

ArrayView BufferView_toArray(BufferView buf) {
    return (ArrayView) {
        .data = (const void*)buf.data,
        .size = buf.size
    };
}

BufferView CharArrayView_toBuffer(ArrayView arr) {
    return (BufferView) {
        .data = (const char*)arr.data,
        .size = arr.size 
    };
}

Buffer CharArray_toBuffer(Array arr) {
    return (Buffer) {
        .data = (char*)arr.data,
        .size = arr.size
    };
}

BufferView Buffer_view(Buffer buf) {
    return (BufferView) {
        .size = buf.size,
        .data = buf.data
    };
}

ArrayView Array_view(Array arr) {
    return (ArrayView) {
        .data = arr.data,
        .size = arr.size
    };
}

bool BufferView_isNull(BufferView buf) {
    return buf.data == NULL;
}

bool Buffer_isNull(Buffer buf) {
    return buf.data == NULL;
}

bool BufferView_isEqual(BufferView buf, BufferView other) {
    if (buf.size != other.size) return false;
    return memcmp(buf.data, other.data, buf.size) == 0;
}

#define toBuffer(arr) ((Buffer) { .data = (char*)&arr[0], .size = sizeof(arr) })
#define toBufferView(arr) ((BufferView) { .data = (const char*)&arr[0], .size = sizeof(arr) })
#define toArray(arr) ((Array) { .data = (void*)&arr[0], .size = ARRSIZE(arr) })
#define toArrayView(arr) ((ArrayView) { .data = (void*)&arr[0], .size = ARRSIZE(arr) })
#define asBuffer(val) ((Buffer) { .data = (void*)&(val), .size = sizeof(val) })
#define asBufferView(val) ((BufferView) { .data = (void*)&(val), .size = sizeof(val) })

#define LocalBuffer(sz) ((Buffer) { .data = (char[sz]) {0}, .size = (sz) })
#define LocalArray(T, sz) ((Array) { .data = (T[sz]) {0}, .size = (sz) })

#define Array_begin(arr) ((arr).data)
#define Array_end(arr, T) ((T*)(arr).data + (arr).size)
