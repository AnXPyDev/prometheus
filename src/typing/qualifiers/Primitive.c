typedef enum {
	PRIMITIVE_QUALIFIER_NONE = 0,

    // Type Qualifiers
    PRIMITIVE_QUALIFIER_CONSTANT,

    // Member qualifiers
    PRIMITIVE_QUALIFIER_OPERATOR_UNARY,
    PRIMITIVE_QUALIFIER_OPERATOR_BINARY,
    PRIMITIVE_QUALIFIER_METHOD,
    PRIMITIVE_QUALIFIER_PURE,
    PRIMTIIVE_QUALIFIER_PARSETIME,

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

bool UnionQualifier_match_reverse(void*, Qualifier);

bool PrimitiveQualifier_match(void *vthis, Qualifier other) {
	if (Qualifier_isUnionQualifier(other)) {
        return UnionQualifier_match_reverse(other.object, PrimitiveQualifier_upcast(this));
	}

    if (Qualifier_isPrimitive(other)) return vthis == other.object;

    return false;
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
    .destroy = &PrimitiveQualifier_destroy,
    .match = &PrimitiveQualifier_match,
};

Qualifier PrimitiveQualifier_upcast(EPrimitiveQualifier T) {
    return (Qualifier) {
        .interface = &IQualifier_PrimitiveQualifier,
        .object = (void*)(intptr_t)T
    };
}

bool Qualifier_isPrimitive(Qualifier this) {
    return this.interface == &IQualifier_PrimitiveQualifier;
}
