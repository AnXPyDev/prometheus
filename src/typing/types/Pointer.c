#define POINTER_TYPE_SIZE (sizeof(void*))

typedef struct {
    Type T;
} PointerType;

PointerType PointerType_new(Type T) {
    return (PointerType) { .T = T };
}

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

#undef this

const IType IType_PointerType = {
    .repr_ = &PointerType_repr,
    .info = &PointerType_info,
};

Type PointerType_upcast(PointerType *this) {
    return (Type) { .interface = &IType_PointerType, .object = this };
}



