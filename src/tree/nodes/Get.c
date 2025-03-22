typedef struct {
	Member *member;
} GetNode;

Node GetNode_upcast(GetNode*);
Node GetNode_create(Member *member, Allocator alc) {
	GetNode *this = Allocator_malloc(alc, sizeof(GetNode));
	this->member = member;
	return GetNode_upcast(this);
}

#define this ((GetNode*)vthis)

void GetNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Get({})", Member_repr(this->member));
}

Type GetNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return Type_copy(this->member->type, alc);
}

void GetNode_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

Node GetNode_copy(void *vthis, Allocator alc) {
	return GetNode_create(this->member, alc);
}

#undef this

const IPrintable IPrintable_GetNode = {
	.print = &GetNode_print
};

Printable GetNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetNode, .object = vthis };
}

INode INode_GetNode = {
	.repr_ = &GetNode_repr,
	.resultType = &GetNode_resultType,
	.destroy = &GetNode_destroy,
	.copy = &GetNode_copy

	// simext initialized at runtime
};

Node GetNode_upcast(GetNode *this) {
	return (Node) { .interface = &INode_GetNode, .object = this };
}
