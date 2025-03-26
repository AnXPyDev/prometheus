void SimCache_create(SimCache *this, Allocator alc) {
	this->alc = alc;
	this->memberlists = HashMap_create(32, alc);
}

typedef struct {
	MemberList *ml;
	MemberListInfo *info;
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

MemberListInfo *SimCache_getMemberList(SimCache *this, MemberList *memberlist) {
	bool existed;

	SimCache_mlItem *item = HashMap_ensure_probe(
		this->memberlists, Hash_fromPtr(memberlist), sizeof(SimCache_mlItem),
		SimCache_mlKey(memberlist), &existed, this->alc 
	);

	if (!existed) {
		item->ml = memberlist;
		item->info = MemberListInfo_generate(memberlist, SIM_MEMORY_ALIGNMENT, this->alc);
	}

	return item->info;
}

