void SimCache_create(SimCache *this, Allocator alc) {
	this->alc = alc;
	HashMap_create(&this->memberlists, sizeof(SimMemberListInfo*));
}

#define MEMALIGN(x) memalign((x), SIM_MEMORY_ALIGNMENT)

SimMemberListInfo *SimMemberListInfo_create(MemberList *memberlist, Allocator alc) {
	Array members = MemberList_members(memberlist);

	SimMemberListInfo *this = Allocator_malloc(alc, sizeof(SimMemberListInfo) + sizeof(SimMemberInfo) * members.size);
	this->size = members.size;
	
	Member **it = members.data;
	Member **end = it + members.size;

	SimMemberInfo *ip = this->info;
	Size offset = 0;

	for (; it < end; it++) {
		Size ts = Type_size((*it)->type);
		*(ip++) = (SimMemberInfo) {
			.type_size = ts,
			.offset = offset
		};
		offset += MEMALIGN(Type_size((*it)->type));
	}

	this->memsize = offset;

	return this;
}

#undef MEMALIGN

SimMemberListInfo *SimCache_getMemberList(SimCache *this, MemberList *memberlist) {
	SimMemberListInfo **infop = HashMap_ensure(
		&this->memberlists, (BufferView) { .data = (char*)&memberlist, .size = sizeof(MemberList*) },
		this->alc
	);

	if (!*infop) {
		*infop = SimMemberListInfo_create(memberlist, this->alc);
	}

	return *infop;
}

Size SimMemberList_getMemberOffset(SimMemberListInfo *this, Member *member) {
	return this->info[member->index].offset;
}
