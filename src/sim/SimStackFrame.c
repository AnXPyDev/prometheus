SimStackFrame *SimStackFrame_create(SimStackFrame *parent, MemberList *memberlist, SimMemberListInfo *mlinfo, Allocator alc) {
	SimStackFrame *this = Allocator_calloc(alc, sizeof(SimStackFrame) + mlinfo->memsize);
	this->parent = parent;
	this->mlinfo = mlinfo;
	this->memberlist = memberlist;
	return this;
}

void *SimStackFrame_getValue(SimStackFrame *this, Member *member) {
	if (member->owner == this->memberlist) {
		return (void*)(this->data + this->mlinfo->info[member->index].offset);
	}
	if (this->parent) return SimStackFrame_getValue(this->parent, member);
	return NULL;
}
