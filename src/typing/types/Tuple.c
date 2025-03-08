typedef struct {
	Size size;
	Type elements[];
} TupleType;

Type TupleType_upcast(TupleType*);
Type ConstTupleType_upcast(TupleType*);
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
	*out_info = TypeInfo_ZERO;
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		TypeInfo ti; Type_info(*it, &ti);
		TypeInfo_add(out_info, &ti);
	}
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

bool TupleType_equal(void *vthis, void *vother) {
	TupleType *other = vother;
	if (this->size != other->size) return false;
	Type *oit = other->elements;
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		if (!Type_equal(*it, *(oit++))) return false;
	}
	return true;
}

void ConstTupleType_destroy(void *vthis, Allocator alc) {}

Type TupleType_constcast(void *vthis) {
	return ConstTupleType_upcast(this);
}

Type TupleType_recast(void *vthis) {
	return TupleType_upcast(this);
}

Type ConstTupleType_copy(void *vthis, Allocator alc) {
	return ConstTupleType_upcast(this);
}

#undef this

const IType IType_TupleType = {
   .repr_ = &TupleType_repr,
   .info = &TupleType_info,
   .copy = &TupleType_copy,
   .destroy = &TupleType_destroy,
	.equal = &TupleType_equal,
	.constcast = &TupleType_constcast,
	.recast = &TupleType_recast,
};

const IType IType_ConstTupleType = {
	.repr_ = &TupleType_repr,
	.info = &TupleType_info,
	.copy = &ConstTupleType_copy,
	.destroy = &ConstTupleType_destroy,
	.equal = &TupleType_equal,
	.constcast = &TupleType_constcast,
	.recast = &TupleType_recast,
};

Type TupleType_upcast(TupleType *this) {
    return (Type) { .interface = &IType_TupleType, .object = this };
}

Type ConstTupleType_upcast(TupleType *this) {
	return (Type) { .interface = &IType_ConstTupleType, .object = this };
}

Type MemberList_type(MemberList *this, Allocator alc) {
	TupleType *tuple = Allocator_malloc(alc, sizeof(TupleType) + sizeof(Type) * this->members.size);
	tuple->size = this->members.size;

	Type *ep = tuple->elements;

	Member **it = Vector_begin(&this->members);
	Member **end = it + this->members.size;

	for (; it < end; it++) {
		*(ep++) = Type_copy((*it)->type, alc);
	}

	return TupleType_upcast(tuple);
}
