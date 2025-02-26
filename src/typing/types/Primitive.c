typedef enum {
    PRIMITIVE_TYPE_VOID = 0,
    
    // meta
    PRIMITIVE_TYPE_TYPE,
    PRIMITIVE_TYPE_IDENTIFIER,

    // primitive
    PRIMITIVE_TYPE_POINTER,
    PRIMITIVE_TYPE_INT,

    PRIMITIVE_TYPE__END
} EPrimitiveType;

const char *EPrimitiveType_REPR[PRIMITIVE_TYPE__END] = {
    [PRIMITIVE_TYPE_VOID] = "PRIMITIVE_TYPE_VOID",
    [PRIMITIVE_TYPE_TYPE] = "PRIMITIVE_TYPE_TYPE",
    [PRIMITIVE_TYPE_IDENTIFIER] = "PRIMITIVE_TYPE_IDENTIFIER",
    [PRIMITIVE_TYPE_INT] = "PRIMITIVE_TYPE_INT"
};

#define this ((EPrimitiveType)(intptr_t)vthis)

void PrimitiveType_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, EPrimitiveType_REPR[this]);
}

#undef this

const IPrintable IPrintable_PrimitiveType = {
    .print = &PrimitiveType_print
};

Printable PrimitiveType_repr(void *vthis) {
    return (Printable) {
        .interface = &IPrintable_PrimitiveType,
        .object = vthis
    };
}

const IType IType_PrimitiveType = {
    .repr_ = &PrimitiveType_repr
};

Type PrimitiveType_upcast(EPrimitiveType T) {
    return (Type) {
        .interface = &IType_PrimitiveType,
        .object = (void*)(intptr_t)T
    };
}
