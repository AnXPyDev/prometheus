typedef struct {
	Vector holders;
	Allocator alc;
} MemberListAllocator;

typedef struct {
	Size index;
	uint32_t rc;
	ArenaAllocator arena;
	MemberList ml;
} MemberListHolder;

void MemberListAllocator_create(MemberListAllocator *this, Allocator alc) {
	this->alc = alc;
	Vector_create(&this->holders, sizeof(MemberListHolder*));
}

MemberListHolder *MemberList_getHolder(MemberList *ml) {
	return (MemberListHolder*)((char*)ml - offsetof(MemberListHolder, ml));
}

MemberList *MemberList_create(MemberListAllocator *mla) {
	MemberListHolder *holder = Allocator_malloc(mla->alc, sizeof(MemberListHolder));

	// TODO adjust chunk size
	ArenaAllocator_create(&holder->arena, mla->alc, 2048);
	MemberList_init(&holder->ml, ArenaAllocator_upcast(&holder->arena));

	holder->index = mla->holders.size;
	holder->rc = 0;
	holder->ml.owner = mla;
	
	*(MemberListHolder**)Vector_push(&mla->holders, mla->alc) = holder;

	return &holder->ml;
}

void MemberListAllocator_destroyHolder(MemberListAllocator *this, MemberListHolder *holder) {
	MemberList_deinit(&holder->ml);
	*(MemberListHolder**)Vector_get(&this->holders, holder->index) = NULL;
	Allocator_free(this->alc, holder);
}

void MemberList_destroy(MemberList *this) {
	MemberListAllocator *mla = this->owner;
	MemberListHolder *holder = MemberList_getHolder(this);

	if (holder->rc <= 1) return;
	holder->rc--;
	MemberListAllocator_destroyHolder(mla, holder);
}

MemberList *MemberList_constcopy(MemberList *this) {
	MemberListHolder *holder = MemberList_getHolder(this);
	holder->rc++;
	return this;
}

void MemberListAllocator_destroy(MemberListAllocator *this) {
	MemberListHolder **it = Vector_begin(&this->holders);
	MemberListHolder **end = Vector_end(&this->holders);
	for (; it < end; it++) {
		MemberListHolder *holder = *it;
		if (!holder) continue;
		holder->rc = 1;
		MemberListAllocator_destroyHolder(this, holder);
	}
	Vector_destroy(&this->holders, this->alc);
}
