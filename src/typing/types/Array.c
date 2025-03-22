typedef struct {
	Size size;	
	Type T;
} ArrayType;

Type ArrayType_upcast(ArrayType*);
Type ArrayType_create(Type T, Size size, Allocator alc) {
    ArrayType *this = Allocator_malloc(alc, sizeof(ArrayType));
    this->T = Type_copy(T, alc);
	 this->size = size;
    return ArrayType_upcast(this);
}

#define this ((ArrayType*)vthis)

void Printable_ArrayType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Array<{%u}; {}>", repr(unsigned int, (unsigned int)this->size), Type_repr(this->T));
}

const IPrintable IPrintable_ArrayType = {
    .print = &Printable_ArrayType_print
};

Printable ArrayType_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_ArrayType, .object = vthis };
}

void ArrayType_info(void *vthis, TypeInfo *out_info) {
    TypeInfo cinfo; Type_info(this->T, &cinfo);
    *out_info = (TypeInfo) {
        .valid = cinfo.valid,
        .abstract = cinfo.abstract,
        .size = cinfo.size * this->size
    };
}

void ArrayType_destroy(void *vthis, Allocator alc) {
    Type_destroy(this->T, alc);
    Allocator_free(alc, vthis);
}

Type ArrayType_copy(void *vthis, Allocator alc) {
    return ArrayType_create(this->T, this->size, alc);
}

bool ArrayType_equal(void *vthis, void *vother) {
    ArrayType *other = vother;
    if (this->size != other->size) return false;
    return Type_equal(this->T, other->T);
}

Hash ArrayType_hash(void *vthis) {
    return Hash_combine(
        Hash_fromIntPtr((intptr_t)&ArrayType_hash),
        Hash_combine(
            Hash_fromSize(this->size),
            Type_hash(this->T)
        )
    );
}

#undef this


const IType IType_ArrayType = {
    .repr_ = &ArrayType_repr,
    .info = &ArrayType_info,
    .copy = &ArrayType_copy,
    .destroy = &ArrayType_destroy,
    .equal = &ArrayType_equal,
    .hash = &ArrayType_hash
};

Type ArrayType_upcast(ArrayType *this) {
    return (Type) { .interface = &IType_ArrayType, .object = this };
}
