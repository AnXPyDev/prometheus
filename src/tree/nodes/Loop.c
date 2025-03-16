typedef struct {
	Node value;
} LoopNode;

Node LoopNode_upcast(LoopNode*);
Node LoopNode_create(Node value, Allocator alc) {
	LoopNode *this = Allocator_malloc(alc, sizeof(LoopNode));
	this->value = value;
	return LoopNode_upcast(this);
}

#define this ((LoopNode*)vthis)

void LoopNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Loop({})", Node_repr(this->value));
}

Type LoopNode_resultType(void *vthis, Allocator alc) {
	return Node_resultType(this->value, alc);
}

void LoopNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->value, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_LoopNode = {
	.print = &LoopNode_print
};

Printable LoopNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_LoopNode, .object = vthis };
}

INode INode_LoopNode = {
	.repr_ = &LoopNode_repr,
	.resultType = &LoopNode_resultType,
	.destroy = &LoopNode_destroy
};

Node LoopNode_upcast(LoopNode *this) {
	return (Node) { .interface = &INode_LoopNode, .object = this };
}
