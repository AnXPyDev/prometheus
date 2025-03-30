typedef struct {
	MemberList *memberlist;
	MemberListInfo *info;
} StructType;

#define STRUCT_MEMORY_ALIGNMENT 4

Type StructType_upcast(StructType*);
Type ConstStructType_upcast(StructType*);

Type StructType_create(MemberList *memberlist, MemberListInfo *info, Allocator alc) {
	StructType *this = Allocator_malloc(alc, sizeof(StructType));
	this->memberlist = memberlist;
	this->info = info;
	return StructType_upcast(this);
}

#define this ((StructType*)vthis)

void Printable_StructType_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Struct<{}>", MemberList_repr(this->memberlist));
}

const IPrintable IPrintable_StructType = {
    .print = &Printable_StructType_print
};

Printable StructType_repr(void *vthis) {
	return (Printable){ .interface = &IPrintable_StructType, .object = vthis };
}

void StructType_info(void *vthis, TypeInfo *out_info) {
	*out_info = this->info->typeinfo;
}

Type StructType_copy(void *vthis, Allocator alc) {
	StructType *copy = Allocator_malloc(alc, sizeof(StructType));
	copy->info = this->info;
	copy->memberlist = this->memberlist;
	return StructType_upcast(copy);
}

void StructType_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

bool StructType_compare(void *vthis, void *vother, bool (*compare_fn)(Type, Type)) {
	StructType *other = vother;
	if (this->memberlist == other->memberlist) return true;

	if (this->info->typeinfo.size != other->info->typeinfo.size) return false;
	if (this->info->size != other->info->size) return false;

	Member *member_a = Vector_begin(&this->memberlist->members);
	Member *member_b = Vector_begin(&other->memberlist->members);

	MemberInfo *info_a = this->info->info;
	MemberInfo *info_b = other->info->info;

	for (Size i = 0; i < this->memberlist->members.size; i++) {
		if (info_a->offset != info_b->offset) return false;
		if (info_a->typeinfo.size != info_b->typeinfo.size) return false;
		if (!compare_fn(member_a->type, member_b->type)) return false;

		info_a++;
		info_b++;
		member_a++;
		member_b++;
	}

	return true;
}

bool StructType_match(void *vthis, Type vother) {
	if (!Type_isStructType(vother)) return false;
	StructType *other = vother.object;
	return StructType_compare(vthis, other, &Type_match);
}

bool StructType_equal(void *vthis, void *vother) {
	return StructType_compare(vthis, vother, &Type_equal);
}

Hash StructType_hash(void *vthis) {
	Hash hash = Hash_combine(
		Hash_fromIntPtr((intptr_t)&StructType_hash),
		this->info->hash
	);

	return hash;
}

void ConstStructType_destroy(void *vthis, Allocator alc) {}

Type StructType_constcast(void *vthis) {
	return ConstStructType_upcast(this);
}

Type StructType_recast(void *vthis) {
	return StructType_upcast(this);
}

Type ConstStructType_copy(void *vthis, Allocator alc) {
	return ConstStructType_upcast(this);
}

#undef this

const IType IType_StructType = {
   .repr_ = &StructType_repr,
   .info = &StructType_info,
   .copy = &StructType_copy,
   .destroy = &StructType_destroy,
	.equal = &StructType_equal,
	.match = &StructType_match,
	.constcast = &StructType_constcast,
	.recast = &StructType_recast,
	.hash = &StructType_hash
};

const IType IType_ConstStructType = {
	.repr_ = &StructType_repr,
	.info = &StructType_info,
	.copy = &ConstStructType_copy,
	.destroy = &ConstStructType_destroy,
	.equal = &StructType_equal,
	.match = &StructType_match,
	.constcast = &StructType_constcast,
	.recast = &StructType_recast,
	.hash = &StructType_hash
};

Type StructType_upcast(StructType *this) {
    return (Type) { .interface = &IType_StructType, .object = this };
}

Type ConstStructType_upcast(StructType *this) {
	return (Type) { .interface = &IType_ConstStructType, .object = this };
}

bool Type_isStructType(Type this) {
	return this.interface == &IType_StructType || this.interface == &IType_ConstStructType;
}

MemberListInfo *StructType_generateMemberListInfo(MemberList *memberlist, Allocator alc) {
	return MemberListInfo_generate(memberlist, STRUCT_MEMORY_ALIGNMENT, alc);
}

ptrdiff_t StructType_getMemberOffset(StructType *this, Member *member) {
	return MemberList_getMemberOffset(this->info, member);
}

Member *StructType_getMember(StructType *this, BufferView identifier) {
	return MemberList_matching(this->memberlist, identifier);
}
