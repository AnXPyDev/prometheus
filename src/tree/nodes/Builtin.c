typedef struct {
	const void *builtin;
	Size size;
	Node nodes[];
} BuiltinNode;

BuiltinNode *BuiltinNode_create(const void *builtin, Array nodes, Allocator alc) {
	BuiltinNode *this = Allocator_malloc(alc, sizeof(BuiltinNode) + sizeof(Node) * nodes.size);
	this->builtin = builtin;
	this->size = nodes.size;
	memcpy(this->nodes, nodes.data, sizeof(Node) * nodes.size);
	return this;
}

#define this ((BuiltinNode*)vthis)

void BuiltinNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "BuiltinNode({}; ", repr(const void*, this->builtin));
	Node *end = this->nodes + this->size;
	Node *it;
	for (it = this->nodes; it < end - 1; it++) {
		PrintFmt(os, "{}, ", Node_repr(*it));
	}
	PrintFmt(os, "{}", Node_repr(*it));
	OutStream_puts(os, ")");
}

Type BuiltinNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return Node_resultType(this->nodes[this->size - 1], alc);
}


#undef this

const IPrintable IPrintable_BuiltinNode = {
	.print = &BuiltinNode_print
};

Printable BuiltinNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_BuiltinNode, .object = vthis };
}

INode INode_BuiltinNode = {
	.repr_ = &BuiltinNode_repr,
	.resultType = &BuiltinNode_resultType,
};

Node BuiltinNode_upcast(BuiltinNode *this) {
	return (Node) { .interface = &INode_BuiltinNode, .object = this };
}
