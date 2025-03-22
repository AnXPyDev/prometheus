void SimCache_create(SimCache *this, Allocator alc) {
	this->alc = alc;
	this->memberlists = HashMap_create(32, alc);
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
		Member *member = *it;
		Size ts = Type_size(member->type);
		bool isAny = Type_equalPrimitive(Type_strip(member->type), PRIMITIVE_TYPE_ANY);

		*(ip++) = (SimMemberInfo) {
			.type_size = ts,
			.offset = offset,
			.flags = (isAny ? MEMBERINFO_FLAG_ANY : 0)
		};
		offset += MEMALIGN(ts);
	}

	this->memsize = offset;

	return this;
}

#undef MEMALIGN

typedef struct {
	MemberList *ml;
	SimMemberListInfo *info;
} SimCache_mlItem;

bool SimCache_match_ml(void *object, void *payload) {
	SimCache_mlItem *item = object;
	MemberList *ml = payload;
	return item->ml == ml;
}

HashMap_Key SimCache_mlKey(MemberList *ml) {
	return (HashMap_Key) {
		.match = &SimCache_match_ml,
		.payload = ml
	};
}

SimMemberListInfo *SimCache_getMemberList(SimCache *this, MemberList *memberlist) {
	bool existed;

	SimCache_mlItem *item = HashMap_ensure_probe(
		this->memberlists, Hash_fromPtr(memberlist), sizeof(SimCache_mlItem),
		SimCache_mlKey(memberlist), &existed, this->alc 
	);

	if (!existed) {
		item->ml = memberlist;
		item->info = SimMemberListInfo_create(memberlist, this->alc);
	}

	return item->info;
}

Size SimMemberList_getMemberOffset(SimMemberListInfo *this, Member *member) {
	return this->info[member->index].offset;
}
