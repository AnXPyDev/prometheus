typedef struct {
	Member *member;
	void *value;
} MemberValuePair;

const MemberValuePair MemberValuePair_NULL = {
	.member = NULL,
	.value = NULL
};

typedef struct {
	Member *member;
	char value[];
} ParserFrame_valueItem;

bool ParserFrame_match_value(void *object, void *payload) {
	ParserFrame_valueItem *val = object;
	Member *member = payload;
	return val->member == member;
}

HashMap_Key ParserFrame_valueKey(Member *member) {
	return (HashMap_Key) {
		.match = &ParserFrame_match_value, .payload = member
	};
}

void ParserFrame_create(ParserFrame *this, ParserFrame *parent, MemberList *ml, Allocator alc) {
	this->alc = alc;
	this->values = HashMap_create(32, alc);
	this->memberlist = ml;
	this->parent = parent;
}

void *ParserFrame_getValue(ParserFrame *this, Member *member) {
	ParserFrame_valueItem *item = HashMap_get(
		this->values, Hash_fromPtr(member), ParserFrame_valueKey(member)
	);

	if (item) return item->value;
	return NULL;
}

void *ParserFrame_ensureValue(ParserFrame *this, Member *member) {
	Size ts = Type_size(member->type);
	bool existed;

	ParserFrame_valueItem *item = HashMap_ensure_probe(
		this->values, Hash_fromPtr(member), sizeof(ParserFrame_valueItem) + ts,
		ParserFrame_valueKey(member), &existed, this->alc
	);

	if (!existed) {
		item->member = member;
	}

	return item->value;
}

ParserFrame *ParserFrame_getMemberOwner(ParserFrame *this, Member *member) {
	if (member->owner == this->memberlist) return this;
	if (this->parent) return ParserFrame_getMemberOwner(this->parent, member);
	return NULL;
}

void ParserFrame_find(ParserFrame *this, BufferView identifier, Vector_Alc out) {
	Member *member = MemberList_matching(this->memberlist, identifier);
	while (member) {
		MemberValuePair *mvp = (MemberValuePair*)Vector_push(out.vec, out.alc);
		mvp->member = member;
		mvp->value = ParserFrame_getValue(this, member);
		member = member->next;
	}
	if (this->parent) ParserFrame_find(this->parent, identifier, out);
}

void ParserFrame_findOneLocal(ParserFrame *this, BufferView identifier, MemberValuePair *out) {
	Member *member = MemberList_matching(this->memberlist, identifier);
	if (member) {
		out->member = member;
		out->value = ParserFrame_getValue(this, member);
	}
}

void ParserFrame_findOne(ParserFrame *this, BufferView identifier, MemberValuePair *out) {
	Member *member = MemberList_matching(this->memberlist, identifier);
	if (member) {
		out->member = member;
		out->value = ParserFrame_getValue(this, member);
		return;
	}
	if (this->parent) ParserFrame_findOne(this->parent, identifier, out);
}
