typedef enum {
	PRIMITIVE_QUALIFIER_NONE = 0,

    PRIMITIVE_QUALIFIER_CONSTANT,

	PRIMITIVE_QUALIFIER__END
} EPrimitiveQualifier;

const char *EPrimitiveQualifier_REPR[PRIMITIVE_QUALIFIER__END] = {
    [PRIMITIVE_QUALIFIER_NONE] = "PRIMITIVE_QUALIFIER_NONE",
    [PRIMITIVE_QUALIFIER_CONSTANT] = "PRIMITIVE_QUALIFIER_CONSTANT",
};

const char *EPrimitiveQualifier_PRETTY[PRIMITIVE_QUALIFIER__END] = {
    [PRIMITIVE_QUALIFIER_NONE] = "none",
    [PRIMITIVE_QUALIFIER_CONSTANT] = "const",
};

Qualifier PrimitiveQualifier_upcast(EPrimitiveQualifier);

#define this ((EPrimitiveQualifier)(intptr_t)vthis)

void PrimitiveQualifier_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, EPrimitiveQualifier_PRETTY[this]);
}

Qualifier PrimitiveQualifier_copy(void *vthis, Allocator alc) {
    return PrimitiveQualifier_upcast(this);
}

Qualifier PrimitiveQualifier_move(void *vthis, Allocator alc, Allocator owner) {
    return PrimitiveQualifier_upcast(this);
}

void PrimitiveQualifier_destroy(void *vthis, Allocator alc) {}

#undef this

const IPrintable IPrintable_PrimitiveQualifier = {
    .print = &PrimitiveQualifier_print
};

Printable PrimitiveQualifier_repr(void *vthis) {
    return (Printable) {
        .interface = &IPrintable_PrimitiveQualifier,
        .object = vthis
    };
}

const IQualifier IQualifier_PrimitiveQualifier = {
    .repr_ = &PrimitiveQualifier_repr,
    .copy = &PrimitiveQualifier_copy,
    .destroy = &PrimitiveQualifier_destroy
};

Qualifier PrimitiveQualifier_upcast(EPrimitiveQualifier T) {
    return (Qualifier) {
        .interface = &IQualifier_PrimitiveQualifier,
        .object = (void*)(intptr_t)T
    };
}
