typedef struct {
	Qualifier Q;
	Type T;
} QualifierType;

QualifierType QualifierType_new(Qualifier Q, Type T) {
    return (QualifierType) { .Q = Q, .T = T };
}

#define this ((QualifierType*)vthis)

void Printable_QualifierType_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Qualifier({}; {})", Qualifier_repr(this->Q), Type_repr(this->T));
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

#undef this

const IType IType_QualifierType = {
    .repr_ = &QualifierType_repr,
    .info = &QualifierType_info,
};

Type QualifierType_upcast(QualifierType *this) {
    return (Type) { .interface = &IType_QualifierType, .object = this };
}
