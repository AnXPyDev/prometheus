typedef struct {
	Member *member;
} GetNode;

#define this ((GetNode*)vthis)

void GetNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "GetNode(");
	Member_print(this->member, os, BufferView_NULL);
	OutStream_puts(os, ")");
}

Type GetNode_resultType(void *vthis, Contract *alc) {
	// TODO add logic to detect early return
	return this->member->type;
}

#undef this

const IPrintable IPrintable_GetNode = {
	.print = &GetNode_print
};

Printable GetNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetNode, .object = vthis };
}

const INode INode_GetNode = {
	.repr_ = &GetNode_repr,
	.resultType = &GetNode_resultType,

	.simext = ISimNode_SequenceNode,
};

Node GetNode_upcast(GetNode *this) {
	return (Node) { .interface = &INode_GetNode, .object = this };
}
