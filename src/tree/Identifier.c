typedef struct {
    Size size;
    char data[];
} Identifier;

BufferView Identifier_view(Identifier*);

#define this ((Identifier*)vthis)

void Printable_Identifier_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_write(os, Identifier_view(this));
}

#undef this

const IPrintable IPrintable_Identifier = {
    .print = &Printable_Identifier_print
};

Printable Identifier_repr(Identifier *this) {
    return (Printable) {
        .interface = &IPrintable_Identifier,
        .object = this
    };
}

BufferView Identifier_view(Identifier *this) {
    return (BufferView) { .size = this->size, .data = this->data };
}

Identifier *Identifier_create(BufferView buf, Allocator alc) {
    Identifier *this = Allocator_malloc(alc, sizeof(Identifier) + buf.size);
    this->size = buf.size;
    memcpy(this->data, buf.data, this->size);
    return this;
}

Identifier *Identifier_copy(Identifier *this, Allocator alc) {
    return Identifier_create(Identifier_view(this), alc);
}

void Identifier_destroy(Identifier *this, Allocator alc) {
    Allocator_free(alc, this);
}


