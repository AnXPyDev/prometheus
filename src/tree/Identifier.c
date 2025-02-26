typedef struct {
    StringView value;
} Identifier;

#define this ((Identifier*)vthis)

void Printable_Identifier_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_write(os, this->value);
}

#undef this

const IPrintable IPrintable_Identifier = {
    .print = &Printable_Identifier_print
};

Printable Identifier_repr(Identifier *this) {
    return (Printable) {
        .interface = &IPrintable_Identifier,
        .object = this
    };
}

Identifier Identifier_new(StringView str) {
    return (Identifier) { .value = str };
}
