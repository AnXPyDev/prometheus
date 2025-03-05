typedef struct {
	Member *member;
} TakePointerNode;

Node TakePointerNode_upcast(TakePointerNode*);
Node TakePointerNode_create(Member *member, Allocator alc) {
	TakePointerNode *this = Allocator_malloc(alc, sizeof(TakePointerNode));
	this->member = member;
	return TakePointerNode_upcast(this);
}

#define this ((TakePointerNode*)vthis)

void TakePointerNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "TakePointer({})", Member_repr(this->member));
}

Type TakePointerNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return PointerType_create(this->member->type, alc);
}

void TakePointerNode_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_TakePointerNode = {
	.print = &TakePointerNode_print
};

Printable TakePointerNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_TakePointerNode, .object = vthis };
}

INode INode_TakePointerNode = {
	.repr_ = &TakePointerNode_repr,
	.resultType = &TakePointerNode_resultType,
	.destroy = &TakePointerNode_destroy,

	// simext initialized at runtime
};

Node TakePointerNode_upcast(TakePointerNode *this) {
	return (Node) { .interface = &INode_TakePointerNode, .object = this };
}
