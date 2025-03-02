typedef struct {
	Member *member;
} GetNode;

#define this ((GetNode*)vthis)

void GetNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "GetNode(");
	Member_print(this->member, os, BufferView_NULL);
	OutStream_puts(os, ")");
}

Type GetNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return Type_copy(this->member->type, alc);
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

	// simext initialized at runtime
};

Node GetNode_upcast(GetNode *this) {
	return (Node) { .interface = &INode_GetNode, .object = this };
}
