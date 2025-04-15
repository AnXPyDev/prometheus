typedef struct {
    const char *begin;
    const char *end;
    const char *ptr;
} StringInStream;

#define this ((StringInStream*)vthis)

bool StringInStream_end(void *vthis) {
    return this->ptr > this->end;
}

int StringInStream_getc(void *vthis) {
    if (this->ptr >= this->end) {
        this->ptr++;
        return EOF;
    }
    int c = *this->ptr;
    this->ptr++;
    return c;
}

Size StringInStream_read(void *vthis, Buffer buf) {
    Size copied = MIN2(buf.size, (Size)(this->end - this->ptr));
    memcpy(buf.data, this->ptr, copied);
    this->ptr += copied;
    return copied;
}


#undef this

const IInStream IStringInStream = {
    .getc = &StringInStream_getc,
    .read = &StringInStream_read,
    .end = &StringInStream_end,
    .close = NULL,
    .info = NULL
};

void StringInStream_create(StringInStream *this, BufferView buf) {
    this->begin = buf.data;
    this->end = buf.data + buf.size;
    this->ptr = buf.data;
}

StringInStream StringInStream_new(BufferView buf) {
    StringInStream this;
    StringInStream_create(&this, buf);
    return this;
}

StringInStream StringInStream_newString(const String *string) {
    return StringInStream_new(String_view(string));
}

InStream StringInStream_upcast(StringInStream *this) {
    return (InStream) {
        .interface = &IStringInStream,
        .object = (void*)this
    };
}
