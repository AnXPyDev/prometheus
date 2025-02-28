SimStackFrame *SimStackFrame_create(FrameNode *frame, Allocator alc) {
	const Size nmemb = frame->ml.nodes.size;

	typedef struct { void *key; Size size; } PAIR;

	PAIR *pairs = Allocator_malloc(alc, sizeof(PAIR) * nmemb);
	TypeInfo ti;

	PAIR *endpair = pairs;
	Size total = 0;
	MemberNode *end = Vector_end(&frame->ml.nodes);
	for (MemberNode *it = Vector_begin(&frame->ml.nodes); it < end; it++) {
		Type_info(it->member.type, &ti);
		*(endpair++) = (PAIR) { .key = &it->member, .size = ti.size };
		total += ti.size;
	}

	SimStackFrame *this = Allocator_calloc(alc, sizeof(SimStackFrame) + total);

	HashMap_create(&this->membermap, alc, sizeof(void*));

	char *data = this->data;
	for (PAIR *it = pairs; it < endpair; it++) {
		*(void**)HashMap_add(&this->membermap, (BufferView) { .size = sizeof(it->key), .data = (char*)&it->key }) = data;
		data += it->size;
	}

	Allocator_free(alc, pairs);

	return this;
}

SimValue SimStackFrame_getValue(SimStackFrame *this, Member *member) {
	void *data = HashMap_get(&this->membermap, (BufferView) { .size = sizeof(Member*), .data = (char*)&member });
	if (data) return (SimValue) { .data = data };
	if (this->parent) return SimStackFrame_getValue(this->parent, member);
	return SimValue_NULL;
}

