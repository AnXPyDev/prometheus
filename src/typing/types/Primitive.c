#define PRIMITIVE_TYPE_INT_SIZE (sizeof(int))
#define PRIMITIVE_TYPE_ABSTRACT_SIZE (sizeof(void*))

typedef enum {
    PRIMITIVE_TYPE_VOID = 0,
    PRIMITIVE_TYPE_ANY,
    PRIMITIVE_TYPE_AUTO,
    
    // meta
    PRIMITIVE_TYPE_TYPE,
    PRIMITIVE_TYPE_NODE,
    PRIMITIVE_TYPE_QUALIFIER,
    PRIMITIVE_TYPE_IDENTIFIER,
    PRIMITIVE_TYPE_MEMBER,
    PRIMITIVE_TYPE_MESSAGE,

    // parser
    PRIMITIVE_TYPE_KEYWORD,
    PRIMITIVE_TYPE_PARSER_INTRIN,

    // primitive
    PRIMITIVE_TYPE_INT,
    PRIMITIVE_TYPE_BOOL,
    PRIMITIVE_TYPE_SIZE,
    PRIMITIVE_TYPE_BYTE,
    PRIMITIVE_TYPE_I32,
    PRIMITIVE_TYPE_U32,
    PRIMITIVE_TYPE_I8,
    PRIMITIVE_TYPE_U8,

    PRIMITIVE_TYPE__END
} EPrimitiveType;

const char *EPrimitiveType_REPR[PRIMITIVE_TYPE__END] = {
    [PRIMITIVE_TYPE_VOID] = "PRIMITIVE_TYPE_VOID",
    [PRIMITIVE_TYPE_ANY] = "PRIMITIVE_TYPE_ANY",
    [PRIMITIVE_TYPE_AUTO] = "PRIMITIVE_TYPE_AUTO",

    [PRIMITIVE_TYPE_TYPE] = "PRIMITIVE_TYPE_TYPE",
    [PRIMITIVE_TYPE_NODE] = "PRIMITIVE_TYPE_NODE",
    [PRIMITIVE_TYPE_QUALIFIER] = "PRIMITIVE_TYPE_QUALIFIER",
    [PRIMITIVE_TYPE_IDENTIFIER] = "PRIMITIVE_TYPE_IDENTIFIER",
    [PRIMITIVE_TYPE_MEMBER] = "PRIMITIVE_TYPE_MEMBER",
    [PRIMITIVE_TYPE_MESSAGE] = "PRIMITIVE_TYPE_MESSAGE",
    
    [PRIMITIVE_TYPE_KEYWORD] = "PRIMITIVE_TYPE_KEYWORD",
    [PRIMITIVE_TYPE_PARSER_INTRIN] = "PRIMITIVE_TYPE_PARSER_INTRIN",

    [PRIMITIVE_TYPE_INT] = "PRIMITIVE_TYPE_INT",
    [PRIMITIVE_TYPE_BOOL] = "PRIMITIVE_TYPE_BOOL",
    [PRIMITIVE_TYPE_SIZE] = "PRIMITIVE_TYPE_SIZE",
    [PRIMITIVE_TYPE_BYTE] = "PRIMITIVE_TYPE_BYTE",
    [PRIMITIVE_TYPE_I32] = "PRIMITIVE_TYPE_I32",
    [PRIMITIVE_TYPE_U32] = "PRIMITIVE_TYPE_U32",
    [PRIMITIVE_TYPE_I8] = "PRIMITIVE_TYPE_I8",
    [PRIMITIVE_TYPE_U8] = "PRIMITIVE_TYPE_U8",
};

const char *EPrimitiveType_PRETTY[PRIMITIVE_TYPE__END] = {
    [PRIMITIVE_TYPE_VOID] = "void",
    [PRIMITIVE_TYPE_ANY] = "<any>",
    [PRIMITIVE_TYPE_AUTO] = "<auto>",

    [PRIMITIVE_TYPE_TYPE] = "<type>",
    [PRIMITIVE_TYPE_NODE] = "<node>",
    [PRIMITIVE_TYPE_QUALIFIER] = "<qualifier>",
    [PRIMITIVE_TYPE_IDENTIFIER] = "<identifier>",
    [PRIMITIVE_TYPE_MEMBER] = "<member>",
    [PRIMITIVE_TYPE_MESSAGE] = "<message>",
    
    [PRIMITIVE_TYPE_KEYWORD] = "<keyword>",
    [PRIMITIVE_TYPE_PARSER_INTRIN] = "<parser_intrin>",

    [PRIMITIVE_TYPE_INT] = "int",
    [PRIMITIVE_TYPE_BOOL] = "bool",
    [PRIMITIVE_TYPE_SIZE] = "size",
    [PRIMITIVE_TYPE_BYTE] = "byte",
    [PRIMITIVE_TYPE_I32] = "i32",
    [PRIMITIVE_TYPE_U32] = "u32",
    [PRIMITIVE_TYPE_I8] = "i8",
    [PRIMITIVE_TYPE_U8] = "u8",
};

const TypeInfo EPrimitiveType_INFO[PRIMITIVE_TYPE__END] = {
    [PRIMITIVE_TYPE_VOID] = { .valid = true, .abstract = false, .size = 0 },
    [PRIMITIVE_TYPE_ANY] = { .valid = true, .abstract = true, .size = sizeof(SimValue) },
    [PRIMITIVE_TYPE_AUTO] = { .valid = true, .abstract = true, .size = 0 },

    [PRIMITIVE_TYPE_TYPE] = { .valid = true, .abstract = true, .size = sizeof(Type) },
    [PRIMITIVE_TYPE_NODE] = { .valid = true, .abstract = true, .size = sizeof(Node) },
    [PRIMITIVE_TYPE_QUALIFIER] = { .valid = true, .abstract = true, .size = sizeof(Qualifier) },
    [PRIMITIVE_TYPE_IDENTIFIER] = { .valid = true, .abstract = true, .size = sizeof(BufferView) },
    [PRIMITIVE_TYPE_MEMBER] = { .valid = true, .abstract = true, .size = sizeof(Member*) },
    [PRIMITIVE_TYPE_MESSAGE] = { .valid = true, .abstract = true, .size = sizeof(const char*) },
    
    [PRIMITIVE_TYPE_KEYWORD] = { .valid = true, .abstract = true, .size = sizeof(int) },
    [PRIMITIVE_TYPE_PARSER_INTRIN] = { .valid = true, .abstract = true, .size = sizeof(void*) },

    [PRIMITIVE_TYPE_INT] = { .valid = true, .abstract = false, .size = sizeof(int) },
    [PRIMITIVE_TYPE_BOOL] = { .valid = true, .abstract = false, .size = sizeof(bool) },
    [PRIMITIVE_TYPE_SIZE] = { .valid = true, .abstract = false, .size = sizeof(Size) },
    [PRIMITIVE_TYPE_BYTE] = { .valid = true, .abstract = false, .size = 1 },
    [PRIMITIVE_TYPE_I32] = { .valid = true, .abstract = false, .size = sizeof(int32_t) },
    [PRIMITIVE_TYPE_U32] = { .valid = true, .abstract = false, .size = sizeof(uint32_t) },
    [PRIMITIVE_TYPE_I8] = { .valid = true, .abstract = false, .size = sizeof(int8_t) },
    [PRIMITIVE_TYPE_U8] = { .valid = true, .abstract = false, .size = sizeof(uint8_t) },
};

Type PrimitiveType_upcast(EPrimitiveType);

bool Type_isPrimitive(Type);

#define this ((EPrimitiveType)(intptr_t)vthis)

void PrimitiveType_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, EPrimitiveType_PRETTY[this]);
}

void PrimitiveType_info(void *vthis, TypeInfo *out_info) {
    *out_info = EPrimitiveType_INFO[this];
}

const IPrintable IPrintable_PrimitiveType = {
    .print = &PrimitiveType_print
};

Printable PrimitiveType_repr(void *vthis) {
    return (Printable) {
        .interface = &IPrintable_PrimitiveType,
        .object = vthis
    };
}

Type PrimitiveType_copy(void *vthis, Allocator alc) {
    return PrimitiveType_upcast(this);
}

void PrimitiveType_destroy(void *vthis, Allocator alc) {}

bool PrimitiveType_equal(void *vthis, void *vother) { return vthis == vother; }

bool PrimitiveType_match(void *vthis, Type other) {
    if (this == PRIMITIVE_TYPE_ANY) return true;
    if (Type_isPrimitive(other)) return vthis == other.object;
    return false;
}

Hash PrimitiveType_hash(void *vthis) {
    return Hash_combine(Hash_fromIntPtr((intptr_t)&PrimitiveType_hash), Hash_fromInt((int)this));
}

#undef this

const IType IType_PrimitiveType = {
    .repr_ = &PrimitiveType_repr,
    .info = &PrimitiveType_info,
    .copy = &PrimitiveType_copy,
    .destroy = &PrimitiveType_destroy,
    .equal = &PrimitiveType_equal,
    .match = &PrimitiveType_match,
    .hash = &PrimitiveType_hash
};

Type PrimitiveType_upcast(EPrimitiveType T) {
    return (Type) {
        .interface = &IType_PrimitiveType,
        .object = (void*)(intptr_t)T
    };
}

EPrimitiveType Type_asPrimitive(Type this) {
    return (EPrimitiveType)(intptr_t)this.object;
}



bool Type_isPrimitive(Type this) {
    return this.interface == &IType_PrimitiveType;
}

bool Type_equalPrimitive(Type this, EPrimitiveType T) {
    if (!Type_isPrimitive(this)) return false;
    return (EPrimitiveType)(intptr_t)this.object == T;
}
