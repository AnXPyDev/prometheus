typedef struct {
	Size size;
	Type elements[];
} TupleType;

Type TupleType_upcast(TupleType*);
Type TupleType_create(Array elements, Allocator alc) {
	TupleType *this = Allocator_malloc(alc, sizeof(TupleType) + sizeof(Type) * elements.size);
	this->size = elements.size;
	Type *ep = elements.data;
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		*it = Type_copy(*(ep++), alc);
	}
	return TupleType_upcast(this);
}

#define this ((TupleType*)vthis)

void Printable_TupleType_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "<");
	Type *end = this->elements + this->size;
	Type *it;
	for (it = this->elements; it < end - 1; it++) {
		PrintFmt(os, "{}, ", Type_repr(*it));
	}
	PrintFmt(os, "{}>", Type_repr(*it));
}

const IPrintable IPrintable_TupleType = {
    .print = &Printable_TupleType_print
};

Printable TupleType_repr(void *vthis) {
	return (Printable){ .interface = &IPrintable_TupleType, .object = vthis };
}

void TupleType_info(void *vthis, TypeInfo *out_info) {
	Size accum = 0;
	out_info->abstract = false;
	out_info->valid = true;

	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		TypeInfo ti; Type_info(*it, &ti);
		if (ti.abstract) out_info->abstract = true;
		if (!ti.valid) out_info->valid = false;
		accum += ti.size;
	}

	out_info->size = accum;
}

Type TupleType_copy(void *vthis, Allocator alc) {
	return TupleType_create((Array) { .data = this->elements, .size = this->size }, alc);
}

void TupleType_destroy(void *vthis, Allocator alc) {
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		Type_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

#undef this

const IType IType_TupleType = {
    .repr_ = &TupleType_repr,
    .info = &TupleType_info,
    .copy = &TupleType_copy,
    .destroy = &TupleType_destroy
};

Type TupleType_upcast(TupleType *this) {
    return (Type) { .interface = &IType_TupleType, .object = this };
}
