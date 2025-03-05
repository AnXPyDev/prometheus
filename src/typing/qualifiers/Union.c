typedef struct {
	Size size;
	Qualifier elements[];	
} UnionQualifier;

Qualifier UnionQualifier_upcast(UnionQualifier*);
Qualifier UnionQualifier_create(Array elements, Allocator alc) {
	UnionQualifier *this = Allocator_malloc(alc, sizeof(UnionQualifier) + sizeof(Qualifier) * elements.size);

	this->size = elements.size;
	Qualifier *elm = this->elements;

	Qualifier *end = (Qualifier*)elements.data + elements.size;
	for (Qualifier *it = elements.data; it < end; it++) {
		*(elm++) = Qualifier_copy(*it, alc);
	}
	
	return UnionQualifier_upcast(this);
}

#define this ((UnionQualifier*)vthis)

void UnionQualifier_print(void *vthis, OutStream os, StringView fmt) {
	Qualifier *end = this->elements + this->size - 1;
	Qualifier *it;
	OutStream_puts(os, "(");
	for (it = this->elements; it < end; it++) {
		Printable_print(Qualifier_repr(*it), os, BufferView_NULL);
		OutStream_puts(os, ", ");
	}
	Printable_print(Qualifier_repr(*it), os, BufferView_NULL);
	OutStream_putc(os, ')');
}

const IPrintable IPrintable_UnionQualifier = {
	.print = &UnionQualifier_print
};

Printable UnionQualifier_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_UnionQualifier, .object = vthis };
}

Qualifier UnionQualifier_copy(void *vthis, Allocator alc) {
	return UnionQualifier_create((Array) { .data = this->elements, .size = this->size  }, alc);
}

void UnionQualifier_destroy(void *vthis, Allocator alc) {
	Qualifier *end = this->elements + this->size;
	for (Qualifier *it = this->elements; it < end; it++) {
		Qualifier_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

#undef this

const IQualifier IQualifier_UnionQualifier = {
	.repr_ = &UnionQualifier_repr,
	.copy = &UnionQualifier_copy,
	.destroy = &UnionQualifier_destroy
};

Qualifier UnionQualifier_upcast(UnionQualifier *this) {
	return (Qualifier) { .interface = &IQualifier_UnionQualifier, .object = this };
}
