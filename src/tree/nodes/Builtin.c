enum {
	BUILTIN_NODE_FLAG_PURE = 1 << 0
};

typedef struct {
	const void *builtin;
	int flags;
	Type result;
	Size size;
	Node nodes[];
} BuiltinNode;

Node BuiltinNode_upcast(BuiltinNode *this);
Node BuiltinNode_create(const void *builtin, Type result, Array nodes, int flags, Allocator alc) {
	BuiltinNode *this = Allocator_malloc(alc, sizeof(BuiltinNode) + sizeof(Node) * nodes.size);
	this->builtin = builtin;
	this->flags = flags;
	this->size = nodes.size;
	this->result = Type_copy(result, alc);
	memcpy(this->nodes, nodes.data, sizeof(Node) * nodes.size);
	return BuiltinNode_upcast(this);
}

#define this ((BuiltinNode*)vthis)

void BuiltinNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Builtin({}; ", repr(ptr_t, this->builtin));
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
	return Type_copy(this->result, alc);
}

void BuiltinNode_destroy(void *vthis, Allocator alc) {
	Node *end = this->nodes + this->size;
	for (Node *it = this->nodes; it < end; it++) {
		Node_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

Node BuiltinNode_copy(void *vthis, Allocator alc) {
	BuiltinNode *copy = Allocator_malloc(alc, sizeof(BuiltinNode) + sizeof(Node) * this->size);
	copy->builtin = this->builtin;
	copy->flags = this->flags;
	copy->size = this->size;
	copy->result = Type_copy(this->result, alc);

	{
		Node *it = this->nodes;
		Node *end = it + this->size;
		Node *dst = copy->nodes;

		for (; it < end; it++) {
			*(dst++) = Node_copy(*it, alc);
		}
	}

	return BuiltinNode_upcast(copy);
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
	.destroy = &BuiltinNode_destroy,
	.copy = &BuiltinNode_copy
};

Node BuiltinNode_upcast(BuiltinNode *this) {
	return (Node) { .interface = &INode_BuiltinNode, .object = this };
}
