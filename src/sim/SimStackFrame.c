SimStackFrame *SimStackFrame_create(Array memberlists, SimMemberListCache *cache, Allocator alc) {
	SimMemberListDescriptor *desc = SimMemberListCache_ensure(cache, memberlists);
	SimStackFrame *this = Allocator_calloc(alc, sizeof(SimStackFrame) + desc->size);
	this->mmap = &desc->mmap;
	return this;
}

void *SimStackFrame_getValue(SimStackFrame *this, Member *member) {
	Size *offset = HashMap_get(this->mmap, (BufferView) { .size = sizeof(Member*), .data = (char*)&member });
	if (!offset) {
		if (this->parent) return SimStackFrame_getValue(this->parent, member);
		return NULL;
	}
	return this->data + *offset;
}
