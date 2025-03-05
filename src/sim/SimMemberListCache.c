typedef struct {
	Size size;
	HashMap mmap;
} SimMemberListDescriptor;

void SimMemberListCache_create(SimMemberListCache *this, Allocator alc) {
	this->alc = alc;
	HashMap_create(&this->map, this->alc, sizeof(SimMemberListDescriptor));
}

SimMemberListDescriptor *SimMemberListCache_ensure(SimMemberListCache *this, Array memberlists) {
	bool had_item;
	SimMemberListDescriptor *item = HashMap_probe_ensure(&this->map, CharArrayView_toBuffer(Array_view(memberlists)), &had_item);

	if (had_item) {
		return item;
	}

	HashMap_create(&item->mmap, this->alc, sizeof(Size));
	
	Size offset = 0;
	for (MemberList **mlp = memberlists.data, **end = mlp + memberlists.size; mlp < end; mlp++) {
		MemberList *ml = *mlp;
		for (MemberNode *it = Vector_begin(&ml->nodes), *end = Vector_end(&ml->nodes); it < end; it++) {
			Size *item_offset = HashMap_add(&item->mmap, (BufferView) {
				.data = (char*)&(Member*) { &it->member }, .size = sizeof(Member*)
			});

			*item_offset = offset;
			offset += Type_size(it->member.type);
		}
	}

	item->size = offset;

	return item;
}

