typedef struct {
	StringView identifier;
	Type type;
} Member;

typedef struct {
	Vector members;
} MemberList;

void MemberList_create(MemberList *this, Allocator allocator) {
	Vector_create(&this->members, allocator, sizeof(Member));
	Vector_init(&this->members, 8);
}

Member *MemberList_get(MemberList *this, StringView identifier) {
	foreach (Member, it, Vector_begin(&this->members), Vector_end(&this->members)) {
		if (BufferView_isEqual(identifier, it->identifier)) return it;
	}

	return NULL;
}

Member *MemberList_push(MemberList *this) {
	return (Member*)Vector_push(&this->members);
}
