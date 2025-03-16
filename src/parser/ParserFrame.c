typedef struct ParserFrame {
	struct ParserFrame *parent;
	MemberList *memberlist;
	HashMap values;
	Allocator alc;
} ParserFrame;

typedef struct {
	Member *member;
	void *value;
} MemberValuePair;

const MemberValuePair MemberValuePair_NULL = {
	.member = NULL,
	.value = NULL
};

void ParserFrame_create(ParserFrame *this, ParserFrame *parent, MemberList *ml, Allocator alc) {
	this->alc = alc;
	HashMap_create(&this->values, sizeof(void*));
	this->memberlist = ml;
	this->parent = parent;
}

void *ParserFrame_getValue(ParserFrame *this, Member *member) {
	void **val = HashMap_get(&this->values, (BufferView) { .data = (char*)&member, .size = sizeof(Member*) });
	if (val) return *val;
	return NULL;
}

void ParserFrame_find(ParserFrame *this, BufferView identifier, Vector *out_members, Allocator out_alc) {
	Member *member = MemberList_matching(this->memberlist, identifier);
	while (member) {
		MemberValuePair *mvp = (MemberValuePair*)Vector_push(out_members, out_alc);
		mvp->member = member;
		mvp->value = ParserFrame_getValue(this, member);
		member = member->next;
	}
	if (this->parent) ParserFrame_find(this->parent, identifier, out_members, out_alc);
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

ParserFrame *ParserFrame_getRoot(ParserFrame *this) {
	ParserFrame *frame = this;
	while (frame->parent) {
		frame = frame->parent;
	}
	return frame;
}
