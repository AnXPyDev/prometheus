typedef struct {
    bool valid;
    bool abstract;
    Size size;
} TypeInfo;

const TypeInfo TypeInfo_NULL = { 0 };
const TypeInfo TypeInfo_ZERO = {
    .valid = true,
    .abstract = false,
    .size = 0
};

void TypeInfo_add(TypeInfo *this, TypeInfo *other) {
    if (other->abstract) this->abstract = true;
    if (!other->valid) this->valid = false;
    this->size += other->size;
}

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

struct Type;

typedef struct {
    Printable (*repr_)(void *this);
    void (*info)(void *this, TypeInfo *out_typeinfo);
    Size (*size)(void *this);
    void (*destroy)(void *this, Allocator alc);
    struct Type (*copy)(void *this, Allocator alc);
    bool (*equal)(void *this, void *other);
    bool (*match)(void *this, struct Type other);
    Hash (*hash)(void *this);
    
    struct Type (*constcast)(void *this);
    struct Type (*recast)(void *this);
} IType;

typedef struct Type {
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
    if (Type_isNull(this)) { *info = TypeInfo_NULL; return; };
    this.interface->info(this.object, info);
}

Size Type_size(Type this)  {
    if (Type_isNull(this)) return 0;
    if (this.interface->size) return this.interface->size(this.object);

    TypeInfo ti; Type_info(this, &ti);
    return ti.size;
}

Type Type_copy(Type this, Allocator alc) {
    if (Type_isNull(this)) return Type_NULL;
    return this.interface->copy(this.object, alc);
}

void Type_destroy(Type this, Allocator alc) {
    if (Type_isNull(this)) return;
    this.interface->destroy(this.object, alc);
}

bool Type_equal(Type this, Type other) {
    bool this_null = Type_isNull(this), other_null = Type_isNull(other);
    if (this_null || other_null) return this_null && other_null;

    if (!this.interface->equal) return false;
    if (this.interface->equal != other.interface->equal) return false;
    return this.interface->equal(this.object, other.object);
}

Type Type_constcast(Type this) {
    if (Type_isNull(this)) return Type_NULL;
    if (!this.interface->constcast) return this;
    return this.interface->constcast(this.object);
}

Type Type_recast(Type this) {
    if (Type_isNull(this)) return Type_NULL;
    if (!this.interface->recast) return this;
    return this.interface->recast(this.object);
}

bool Type_match(Type this, Type other) {
    if (Type_isNull(this)) return false;
    if (!this.interface->match) return Type_equal(this, other);
    return this.interface->match(this.object, other);
}

bool Type_nullOrMatch(Type this, Type other) {
    if (Type_isNull(this)) return true;
    return Type_match(this, other);
}

Hash Type_hash(Type this) {
    if (!this.interface->match) return Hash_NULL;
    return this.interface->hash(this.object);
}

bool Type_isPrimitive(Type);
bool Type_isPointerType(Type);
bool Type_isQualifierType(Type);
bool Type_isFunctionType(Type);
bool Type_isSpanType(Type);
bool Type_isArrayType(Type);
bool Type_isTupleType(Type);
bool Type_isStructType(Type);

Type Type_strip(Type);
