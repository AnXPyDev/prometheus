typedef struct {
	Qualifier Q;
	Type T;
} QualifierType;

Type QualifierType_upcast(QualifierType*);
Type QualifierType_create(Qualifier Q, Type T, Allocator alc) {
    QualifierType *this = Allocator_malloc(alc, sizeof(QualifierType));
    this->Q = Qualifier_copy(Q, alc);
    this->T = Type_copy(T, alc);
    return QualifierType_upcast(this);
}

#define this ((QualifierType*)vthis)

void Printable_QualifierType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "{}<{}>", Qualifier_repr(this->Q), Type_repr(this->T));
}

const IPrintable IPrintable_QualifierType = {
    .print = &Printable_QualifierType_print
};

Printable QualifierType_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_QualifierType, .object = vthis };
}

void QualifierType_info(void *vthis, TypeInfo *out_info) {
    Type_info(this->T, out_info);
}

Type QualifierType_copy(void *vthis, Allocator alc) {
    QualifierType *copy = Allocator_malloc(alc, sizeof(QualifierType));
    copy->Q = Qualifier_copy(this->Q, alc);
    copy->T = Type_copy(this->T, alc);
    return QualifierType_upcast(copy);
}

void QualifierType_destroy(void *vthis, Allocator alc) {
    Qualifier_destroy(this->Q, alc);
    Type_destroy(this->T, alc);
    Allocator_free(alc, vthis);
}

bool QualifierType_equal(void *vthis, void *vother) {
    // TODO check qualifier equality
    QualifierType *other = vother;
    return Type_equal(this->T, other->T);
}

bool QualifierType_match(void *vthis, Type other) {
    return Type_match(this->T, other);
}

#undef this

const IType IType_QualifierType = {
    .repr_ = &QualifierType_repr,
    .info = &QualifierType_info,
    .copy = &QualifierType_copy,
    .destroy = &QualifierType_destroy,
    .equal = &QualifierType_equal,
    .match = &QualifierType_match
};

Type QualifierType_upcast(QualifierType *this) {
    return (Type) { .interface = &IType_QualifierType, .object = this };
}

bool Type_isQualifierType(Type this) {
    return this.interface == &IType_QualifierType;
}

Type Type_strip(Type this) {
    if (!Type_isQualifierType(this)) {
        return this;
    }

    QualifierType *qthis = this.object;
    return Type_strip(qthis->T);
}
