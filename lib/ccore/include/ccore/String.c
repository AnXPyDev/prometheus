#ifdef CCORE_STRING_ZERO_COMPAT
#define STRING_ZERO true
#define STRING_PAD 1
#else
#define STRING_ZERO false
#define STRING_PAD 0
#endif

typedef struct {
    Size length;
    Size capacity;
    char *data;
} String;

StringView String_view(const String *this) {
    return (StringView) {
        .data = this->data,
        .size = this->length
    };
}

const String String_NULL = {
    .length = 0,
    .capacity = 0,
    .data = NULL
};


#define String_begin(this) ((this)->data)
#define String_end(this) ((this)->data + (this)->length)

bool String_isNull(String *this) {
    return this->data == NULL;
}

void String_destroy(String *this, Allocator alc) {
    this->capacity = 0;
    this->length = 0;
    Allocator_free(alc, this->data);
    this->data = NULL;
}

void String_zero(String *this) {
    if (!STRING_ZERO) {
        return;
    }

    this->data[this->length] = 0;
}

void String_blank(String *this, Size capacity, Allocator alc) {
    this->data = Allocator_malloc(alc, capacity + STRING_PAD);
    this->capacity = capacity;
    this->length = 0;
    String_zero(this);
}

void String_copy(String *this, StringView other, Allocator alc) {
    this->data = Allocator_malloc(alc, other.size + STRING_PAD);
    this->length = other.size;
    this->capacity = other.size;
    memcpy(this->data, other.data, other.size);
    String_zero(this);
}

void String_clear(String *this) {
    this->length = 0;
    String_zero(this);
}


void String_append(String *this, StringView other, Allocator alc) {
    Size newlen = other.size + this->length;
    if (newlen > this->capacity) {
        this->data = Allocator_realloc(alc, this->data, newlen + STRING_PAD);
        this->capacity = newlen;
    }
    memcpy(String_end(this), other.data, other.size);
    this->length = newlen;
    String_zero(this);
}

#define this ((String*)vthis)

void Printable_String_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_write(os, String_view(this));
}

#undef this

const IPrintable IPrintable_String = {
    .print = &Printable_String_print
};

Printable String_repr(String *this) {
    return (Printable) { .object = (void*)this, .interface = &IPrintable_String };
}

#undef STRING_PAD
#undef STRING_ZERO
