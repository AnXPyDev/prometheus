typedef struct {
    Type T;
} SpanType;

Type SpanType_upcast(SpanType*);
Type SpanType_create(Type T, Allocator alc) {
    SpanType *this = Allocator_malloc(alc, sizeof(SpanType));
    this->T = Type_copy(T, alc);
    return SpanType_upcast(this);
}

#define this ((SpanType*)vthis)

void Printable_SpanType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Span<{}>", Type_repr(this->T));
}

const IPrintable IPrintable_SpanType = {
    .print = &Printable_SpanType_print
};

Printable SpanType_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_SpanType, .object = vthis };
}

void SpanType_info(void *vthis, TypeInfo *out_info) {
    TypeInfo cinfo; Type_info(this->T, &cinfo);
    *out_info = (TypeInfo) {
        .valid = cinfo.valid,
        .abstract = cinfo.abstract,
        .size = POINTER_TYPE_SIZE * 2
    };
}

void SpanType_destroy(void *vthis, Allocator alc) {
    Type_destroy(this->T, alc);
    Allocator_free(alc, vthis);
}

Type SpanType_copy(void *vthis, Allocator alc) {
    return SpanType_create(this->T, alc);
}

bool SpanType_equal(void *vthis, void *vother) {
    SpanType *other = vother;
    return Type_equal(this->T, other->T);
}

bool SpanType_match(void *vthis, Type other) {
    if (!Type_isSpanType(other)) return false;
    SpanType *sother = other.object;
    return Type_match(this->T, sother->T);
}

Hash SpanType_hash(void *vthis) {
    return Hash_combine(
        Hash_fromIntPtr((intptr_t)&SpanType_hash),
        Type_hash(this->T)
    );
}

#undef this

const IType IType_SpanType = {
    .repr_ = &SpanType_repr,
    .info = &SpanType_info,
    .copy = &SpanType_copy,
    .destroy = &SpanType_destroy,
    .equal = &SpanType_equal,
    .hash = &SpanType_hash
};

Type SpanType_upcast(SpanType *this) {
    return (Type) { .interface = &IType_SpanType, .object = this };
}

bool Type_isSpanType(Type this) {
    return this.interface == &IType_SpanType;
}

