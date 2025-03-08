SimStackFrame *SimStackFrame_create(SimStackFrame *parent, MemberList *memberlist, SimMemberListInfo *mlinfo, Allocator alc) {
	SimStackFrame *this = Allocator_malloc(alc, sizeof(SimStackFrame) + mlinfo->memsize);
	this->mlinfo = mlinfo;
	this->memberlist = memberlist;
	return this;
}

void *SimStackFrame_getValue(SimStackFrame *this, Member *member) {
	if (member->owner == this->memberlist) {
		return (void*)(this->data + this->mlinfo->offsets[member->index]);
	}
	if (this->parent) return SimStackFrame_getValue(this->parent, member);
	return NULL;
}
