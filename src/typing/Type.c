typedef struct {
    bool valid;
    bool abstract;
    Size size;
} TypeInfo;

const TypeInfo TypeInfo_NULL = { 0 };

#define this ((TypeInfo*)vthis)

void TypeInfo_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "TypeInfo({}{}size: {%llu})",
        strrepr(this->valid ? "" : "invalid, "),
        strrepr(this->abstract ? "abstract, " : ""),
        repr(Size, this->size)
    );
}

#undef this

const IPrintable IPrintable_TypeInfo = {
    .print = &TypeInfo_print
};

Printable TypeInfo_repr(TypeInfo *this) {
    return (Printable) { .interface = &IPrintable_TypeInfo, .object = this };
}

typedef struct {
    Printable (*repr_)(void *this);
    void (*info)(void *this, TypeInfo *out_typeinfo);
} IType;

typedef struct {
    const IType *interface;
    void *object;
} Type;

const Type Type_NULL = { 0 };

bool Type_isNull(Type this) { return this.interface == NULL; }

Printable Type_repr(Type this) {
    if (Type_isNull(this)) return Printable_NULL;
    return this.interface->repr_(this.object);
}

void Type_info(Type this, TypeInfo *info) {
    if (Type_isNull(this)) return;
    this.interface->info(this.object, info);
}
