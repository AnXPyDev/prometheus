typedef struct {
    Type T;
} PointerType;

PointerType PointerType_wrap(Type T) {
    return (PointerType) { .T = T };
}

#define this ((PointerType*)vthis)

void Printable_PointerType_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, "POINTER[");
    Printable_print(Type_repr(this->T), os, fmt);
    OutStream_puts(os, "]");
}

const IPrintable IPrintable_PointerType = {
    .print = &Printable_PointerType_print
};

Printable PointerType_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_PointerType, .object = vthis };
}

#undef this

const IType IType_PointerType = {
    .repr_ = &PointerType_repr
};

Type PointerType_upcast(PointerType *this) {
    return (Type) { .interface = &IType_PointerType, .object = this };
}



