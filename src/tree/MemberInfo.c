MemberListInfo *MemberListInfo_generate(MemberList *memberlist, Size align, Allocator alc) {
	Array members = MemberList_members(memberlist);

	MemberListInfo *this = Allocator_malloc(alc, sizeof(MemberListInfo) + sizeof(MemberInfo) * members.size);
	this->size = members.size;
	this->typeinfo = TypeInfo_ZERO;

	Hash mlhash = Hash_NULL;
	
	Member **it = members.data;
	Member **end = it + members.size;

	MemberInfo *ip = this->info;
	Size offset = 0;

	for (; it < end; it++) {
		Member *member = *it;
		TypeInfo ti = TypeInfo_NULL; Type_info(member->type, &ti);
		TypeInfo_add(&this->typeinfo, &ti);
		Hash hash = Type_hash(member->type);

		mlhash = Hash_combine(mlhash, hash);

		bool isAny = Type_equalPrimitive(Type_strip(member->type), PRIMITIVE_TYPE_ANY);

		*(ip++) = (MemberInfo) {
			.typeinfo = ti,
			.offset = (ptrdiff_t)offset,
			.hash = hash,
			.flags = (isAny ? MEMBERINFO_FLAG_ANY : 0)
		};
		offset += memalign(ti.size, align);
	}

	this->typeinfo.size = offset;
	this->hash = mlhash;

	return this;
}

ptrdiff_t MemberList_getMemberOffset(MemberListInfo *this, Member *member) {
	return this->info[member->index].offset;
}
