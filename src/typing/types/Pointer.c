#define POINTER_TYPE_SIZE (sizeof(void*))

typedef struct {
    Type T;
} PointerType;

PointerType PointerType_new(Type T) {
    return (PointerType) { .T = T };
}

PointerType *PointerType_create(Type T, Allocator alc) {
    PointerType *this = Allocator_malloc(alc, sizeof(PointerType));
    this->T = Type_copy(T, alc);
    return this;
}

Type PointerType_upcast(PointerType*);

#define this ((PointerType*)vthis)

void Printable_PointerType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Pointer({})", Type_repr(this->T));
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
    return PointerType_upcast(PointerType_create(this->T, alc));
}

Type PointerType_move(void *vthis, Allocator alc, Allocator owner) {
    Type result = PointerType_copy(vthis, alc);
    PointerType_destroy(vthis, owner);
    return result;
}


#undef this


const IType IType_PointerType = {
    .repr_ = &PointerType_repr,
    .info = &PointerType_info,
    .copy = &PointerType_copy,
    .move = &PointerType_move,
    .destroy = &PointerType_destroy
};

Type PointerType_upcast(PointerType *this) {
    return (Type) { .interface = &IType_PointerType, .object = this };
}

