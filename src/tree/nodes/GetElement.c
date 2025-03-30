typedef struct {
	Member *member;
	ptrdiff_t offset;
	Type T;
} GetElementNode;

Node GetElementNode_upcast(GetElementNode*);
Node GetElementNode_create(Member *member, ptrdiff_t offset, Type T, Allocator alc) {
	GetElementNode *this = Allocator_malloc(alc, sizeof(GetElementNode));
	this->member = member;
	this->offset = offset;
	this->T = Type_copy(T, alc);
	return GetElementNode_upcast(this);
}

#define this ((GetElementNode*)vthis)

void GetElementNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "GetElement({}[{}].<{}>)", 
		Member_repr(this->member),
		repr(ptrdiff_t, this->offset),
		Type_repr(this->T)
	);
}

Type GetElementNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return Type_constcast(this->T);
}

void GetElementNode_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

Node GetElementNode_copy(void *vthis, Allocator alc) {
	return GetElementNode_create(this->member, this->offset, this->T, alc);
}

#undef this

const IPrintable IPrintable_GetElementNode = {
	.print = &GetElementNode_print
};

Printable GetElementNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetElementNode, .object = vthis };
}

INode INode_GetElementNode = {
	.repr_ = &GetElementNode_repr,
	.resultType = &GetElementNode_resultType,
	.destroy = &GetElementNode_destroy,
	.copy = &GetElementNode_copy

	// simext initialized at runtime
};

Node GetElementNode_upcast(GetElementNode *this) {
	return (Node) { .interface = &INode_GetElementNode, .object = this };
}
