typedef struct {
    Allocator alc;
    Vector buffer;
} StringOutStream;

#define this ((StringOutStream*)vthis)

void StringOutStream_putc(void *vthis, int c) {
    *(char*)Vector_push(&this->buffer, this->alc) = (char)c;
}

void StringOutStream_write(void *vthis, BufferView buf) {
    Vector_append(&this->buffer, BufferView_toArray(buf), this->alc);
}

void StringOutStream_puts(void *vthis, const char *str) {
    StringOutStream_write(vthis, strview(str));
}

void StringOutStream_close(void *vthis) {
    Vector_destroy(&this->buffer, this->alc);
}

#undef this

const IOutStream IStringOutStream = {
    .putc = StringOutStream_putc,
    .puts = StringOutStream_puts,
    .write = StringOutStream_write,
    .close = StringOutStream_close
};

void StringOutStream_create(StringOutStream *this, Allocator allocator) {
    this->alc = allocator;
    Vector_create(&this->buffer, sizeof(char));
    Vector_init(&this->buffer, 32, this->alc);
}

StringOutStream StringOutStream_new(Allocator allocator) {
    StringOutStream this;
    StringOutStream_create(&this, allocator);
    return this;
}

OutStream StringOutStream_upcast(StringOutStream *this) {
    return (OutStream) {
        .interface = &IStringOutStream,
        .object = (void*)this
    };
}

BufferView StringOutStream_view(StringOutStream *this) {
    return Vector_bufferView(&this->buffer);
}

void StringOutStream_clear(StringOutStream *this) {
    Vector_clear(&this->buffer);
}

void StringOutStream_destroy(StringOutStream *this) {
    Vector_destroy(&this->buffer, this->alc);
}
