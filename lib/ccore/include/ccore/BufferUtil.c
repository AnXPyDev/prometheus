Buffer Buffer_alloc(Size capacity, Allocator allocator) {
    return (Buffer) {
        .size = capacity,
        .data = (char*)Allocator_calloc(allocator, capacity)
    };
}

void Buffer_free(Buffer buffer, Allocator allocator) {
    Allocator_free(allocator, (void*)buffer.data);
}

Array Array_alloc_(Size item_size, Size capacity, Allocator allocator) {
    return (Array) {
        .size = capacity,
        .data = (void*)Allocator_calloc(allocator, capacity * item_size)
    };
}

#define Array_alloc(T, capacity, allocator) Array_alloc_(sizeof(T), (capacity), (allocator))

void Array_free(Array array, Allocator allocator) {
    Allocator_free(allocator, array.data);
}

Buffer Buffer_copy(BufferView this, Allocator allocator) {
    if (BufferView_isNull(this)) {
        return Buffer_NULL;
    }
    Buffer result = Buffer_alloc(this.size, allocator);
    memcpy(result.data, this.data, this.size);
    return result;
}
