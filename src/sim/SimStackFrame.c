SimStackFrame *SimStackFrame_create(SimStackFrame *parent, MemberList *memberlist, SimMemberListInfo *mlinfo, Allocator alc, Allocator tmp_alc) {
	SimStackFrame *this = Allocator_calloc(alc, sizeof(SimStackFrame) + mlinfo->memsize);
	Vector_create(&this->deferred, sizeof(Node));
	this->parent = parent;
	this->mlinfo = mlinfo;
	this->memberlist = memberlist;
	this->deferred_alc = tmp_alc;
	
	Vector_init(&this->deferred, 4, this->deferred_alc);
	return this;
}

void SimStackFrame_destroy(SimStackFrame *this, Allocator alc) {
	Vector_destroy(&this->deferred, this->deferred_alc);
	Allocator_free(alc, this);
}

void *SimStackFrame_getValue(SimStackFrame *this, Member *member) {
	if (member->owner == this->memberlist) {
		return (void*)(this->data + this->mlinfo->info[member->index].offset);
	}
	if (this->parent) return SimStackFrame_getValue(this->parent, member);
	return NULL;
}

void SimStackFrame_evaluateDeferred(SimStackFrame *this, SimContext *ctx, SimResult *out) {
	Node *it = Vector_begin(&this->deferred);
	Node *end = Vector_end(&this->deferred);

	for (; it < end; it++) {
		SimResult result = SimResult_NULL;
		SimNode_evaluate(*it, ctx, &result);
		if (result.control) {
			*out = SimResult_NULL;
			SimResult_forward(&result, out);
		}
	}
}
