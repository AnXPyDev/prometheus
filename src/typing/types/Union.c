typedef struct {
	Size size;
	Type elements[];
} UnionType;

Type UnionType_upcast(UnionType*);
Type UnionType_create(Array elements, Allocator alc) {
	UnionType *this = Allocator_malloc(alc, sizeof(UnionType) + sizeof(Type) * elements.size);
	this->size = elements.size;
	Type *ep = elements.data;
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		*it = Type_copy(*(ep++), alc);
	}
	return UnionType_upcast(this);
}

Type UnionType_create_move(Array elements, Allocator alc) {
	UnionType *this = Allocator_malloc(alc, sizeof(UnionType) + sizeof(Type) * elements.size);
	this->size = elements.size;
	Type *ep = elements.data;
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		*it = *(ep++);
	}
	return UnionType_upcast(this);
}

#define this ((UnionType*)vthis)

void Printable_UnionType_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "Union<");
	Type *end = this->elements + this->size;
	Type *it;
	for (it = this->elements; it < end - 1; it++) {
		PrintFmt(os, "{}, ", Type_repr(*it));
	}
	PrintFmt(os, "{}>", Type_repr(*it));
}

const IPrintable IPrintable_UnionType = {
    .print = &Printable_UnionType_print
};

Printable UnionType_repr(void *vthis) {
	return (Printable){ .interface = &IPrintable_UnionType, .object = vthis };
}

void UnionType_info(void *vthis, TypeInfo *out_info) {
	Size maxsize = 0;
	out_info->abstract = false;
	out_info->valid = true;

	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		TypeInfo ti; Type_info(*it, &ti);
		if (ti.abstract) out_info->abstract = true;
		if (!ti.valid) out_info->valid = false;
		if (ti.size > maxsize) {
			maxsize = ti.size;
		}
	}

	out_info->size = maxsize;
}

Type UnionType_copy(void *vthis, Allocator alc) {
	return UnionType_create((Array) { .data = this->elements, .size = this->size }, alc);
}

void UnionType_destroy(void *vthis, Allocator alc) {
	Type *end = this->elements + this->size;
	for (Type *it = this->elements; it < end; it++) {
		Type_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

#undef this

const IType IType_UnionType = {
    .repr_ = &UnionType_repr,
    .info = &UnionType_info,
    .copy = &UnionType_copy,
    .destroy = &UnionType_destroy
};

Type UnionType_upcast(UnionType *this) {
    return (Type) { .interface = &IType_UnionType, .object = this };
}
