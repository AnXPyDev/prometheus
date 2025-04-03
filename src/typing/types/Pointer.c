#define POINTER_TYPE_SIZE (sizeof(void*))

typedef struct {
    Type T;
} PointerType;

Type ConstPointerType_upcast(PointerType*);
Type PointerType_upcast(PointerType*);
Type PointerType_create(Type T, Allocator alc) {
    PointerType *this = Allocator_malloc(alc, sizeof(PointerType));
    this->T = Type_copy(T, alc);
    return PointerType_upcast(this);
}

#define this ((PointerType*)vthis)

void Printable_PointerType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "@<{}>", Type_repr(this->T));
}

const IPrintable IPrintable_PointerType = {
    .print = &Printable_PointerType_print
};

Printable PointerType_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_PointerType, .object = vthis };
}

void PointerType_info(void *vthis, TypeInfo *out_info) {
    TypeInfo cinfo; Type_info(this->T, &cinfo);
    *out_info = (TypeInfo) {
        .valid = cinfo.valid,
        .abstract = cinfo.abstract,
        .size = POINTER_TYPE_SIZE
    };
}

void PointerType_destroy(void *vthis, Allocator alc) {
    Type_destroy(this->T, alc);
    Allocator_free(alc, vthis);
}

Type PointerType_copy(void *vthis, Allocator alc) {
    return PointerType_create(this->T, alc);
}

Type ConstPointerType_copy(void *vthis, Allocator alc) {
    return ConstPointerType_upcast(this);
}

void ConstPointerType_destroy(void *vthis, Allocator alc) {}

Type PointerType_constcast(void *vthis) {
    return ConstPointerType_upcast(this);
}

Type PointerType_recast(void *vthis) {
    return PointerType_upcast(this);
}

bool PointerType_equal(void *vthis, void *vother) {
    PointerType *other = vother;
    return Type_equal(this->T, other->T);
}

bool PointerType_match(void *vthis, Type other) {
    Type sother = Type_strip(other);
    if (!Type_isPointerType(sother)) return false;
    PointerType *pother = sother.object;
    return Type_match(this->T, pother->T);
}

Hash PointerType_hash(void *vthis) {
    return Hash_combine(
        Hash_fromIntPtr((intptr_t)&PointerType_hash),
        Type_hash(this->T)
    );
}

#undef this

const IType IType_PointerType = {
    .repr_ = &PointerType_repr,
    .info = &PointerType_info,
    .copy = &PointerType_copy,
    .destroy = &PointerType_destroy,
    .equal = &PointerType_equal,
    .match = &PointerType_match,
    .hash = &PointerType_hash,
    .constcast = &PointerType_constcast,
    .recast = &PointerType_recast
};

const IType IType_ConstPointerType = {
    .repr_ = &PointerType_repr,
    .info = &PointerType_info,
    .copy = &ConstPointerType_copy,
    .destroy = &ConstPointerType_destroy,
    .equal = &PointerType_equal,
    .match = &PointerType_match,
    .hash = &PointerType_hash,
    .constcast = &PointerType_constcast,
    .recast = &PointerType_recast
};

Type PointerType_upcast(PointerType *this) {
    return (Type) { .interface = &IType_PointerType, .object = this };
}

Type ConstPointerType_upcast(PointerType *this) {
    return (Type) { .interface = &IType_ConstPointerType, .object = this };
}

bool Type_isPointerType(Type this) {
    return this.interface == &IType_PointerType || this.interface == &IType_ConstPointerType;
}

