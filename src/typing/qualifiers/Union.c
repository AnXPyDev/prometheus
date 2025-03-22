typedef struct {
	Size size;
	Qualifier elements[];	
} UnionQualifier;

Qualifier UnionQualifier_upcast(UnionQualifier*);
Qualifier ConstUnionQualifier_upcast(UnionQualifier*);

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

Qualifier ConstUnionQualifier_copy(void *vthis, Allocator alc) {
	return UnionQualifier_upcast(this);
}

void UnionQualifier_destroy(void *vthis, Allocator alc) {
	Qualifier *end = this->elements + this->size;
	for (Qualifier *it = this->elements; it < end; it++) {
		Qualifier_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

void ConstUnionQualifier_destroy(void *vthis, Allocator alc) {}

Qualifier UnionQualifier_constcast(void *vthis) {
	return ConstUnionQualifier_upcast(this);
}

Qualifier UnionQualifier_recast(void *vthis) {
	return UnionQualifier_upcast(this);
}

bool UnionQualifier_match_reverse(void *vthis, Qualifier other) {
	Qualifier *it = this->elements;
	Qualifier *end = it + this->size;
	for (; it < end; it++) {
		if (!Qualifier_match(other, UnionQualifier_upcast(this))) return false;
	}
	return true;
}

bool UnionQualifier_match(void *vthis, Qualifier other) {
	if (Qualifier_isUnionQualifier(other)) {
		return UnionQualifier_match_reverse(other.object, UnionQualifier_upcast(this));
	}
		
	Qualifier *it = this->elements;
	Qualifier *end = it + this->size;
	for (; it < end; it++) {
		if (!Qualifier_match(*it, other)) return false;
	}
	return true;

}

Hash UnionQualifier_hash(void *vthis) {
	Hash hash = Hash_fromIntPtr((intptr_t)&UnionQualifier_hash);
	Qualifier *it = this->elements;
	Qualifier *end = it + this->size;
	for (; it < end; it++) {
		hash = Hash_combine(hash, Qualifier_hash(*it));
	}
	return hash;
}

#undef this

const IQualifier IQualifier_UnionQualifier = {
	.repr_ = &UnionQualifier_repr,
	.copy = &UnionQualifier_copy,
	.destroy = &UnionQualifier_destroy,
	.constcast = &UnionQualifier_constcast,
	.recast = &UnionQualifier_recast,
	.match = &UnionQualifier_match,
	.hash = &UnionQualifier_hash,
};

const IQualifier IQualifier_ConstUnionQualifier = {
	.repr_ = &UnionQualifier_repr,
	.copy = &ConstUnionQualifier_copy,
	.destroy = &ConstUnionQualifier_destroy,
	.constcast = &UnionQualifier_constcast,
	.recast = &UnionQualifier_recast,
	.match = &UnionQualifier_match,
	.hash = &UnionQualifier_hash,
};

Qualifier UnionQualifier_upcast(UnionQualifier *this) {
	return (Qualifier) { .interface = &IQualifier_UnionQualifier, .object = this };
}

Qualifier ConstUnionQualifier_upcast(UnionQualifier *this) {
	return (Qualifier) { .interface = &IQualifier_ConstUnionQualifier, .object = this };
}

bool Qualifier_isUnionQualifier(Qualifier this) {
	return this.interface == &IQualifier_UnionQualifier
		|| this.interface == &IQualifier_ConstUnionQualifier;
}
