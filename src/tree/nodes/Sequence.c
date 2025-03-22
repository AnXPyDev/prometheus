typedef struct {
	Size size;
	Node nodes[];
} SequenceNode;

Node SequenceNode_upcast(SequenceNode*);
Node SequenceNode_create(Array nodes, Allocator alc) {
	SequenceNode *this = Allocator_malloc(alc, sizeof(SequenceNode) + sizeof(Node) * nodes.size);
	this->size = nodes.size;
	memcpy(this->nodes, nodes.data, sizeof(Node) * nodes.size);
	return SequenceNode_upcast(this);
}

#define this ((SequenceNode*)vthis)

void SequenceNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "Sequence(");
	Node *end = this->nodes + this->size;
	Node *it;
	for (it = this->nodes; it < end - 1; it++) {
		PrintFmt(os, "{}; ", Node_repr(*it));
	}
	PrintFmt(os, "{})", Node_repr(*it));
}

Type SequenceNode_resultType(void *vthis, Allocator alc) {
	// TODO add logic to detect early return
	return Node_resultType(this->nodes[this->size - 1], alc);
}

void SequenceNode_destroy(void *vthis, Allocator alc) {
	Node *end = this->nodes + this->size;
	for (Node *it = this->nodes; it < end; it++) {
		Node_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

Node SequenceNode_copy(void *vthis, Allocator alc) {
	SequenceNode *copy = Allocator_malloc(alc, sizeof(SequenceNode) + sizeof(Node) * this->size);
	{
		Node *it = this->nodes;
		Node *end = it + this->size;
		Node *dst = copy->nodes;

		for (; it < end; it++) {
			*(dst++) = Node_copy(*it, alc);
		}
	}

	return SequenceNode_upcast(copy);
}

#undef this

const IPrintable IPrintable_SequenceNode = {
	.print = &SequenceNode_print
};

Printable SequenceNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_SequenceNode, .object = vthis };
}

INode INode_SequenceNode = {
	.repr_ = &SequenceNode_repr,
	.resultType = &SequenceNode_resultType,
	.destroy = &SequenceNode_destroy,
	.copy = &SequenceNode_copy
};

Node SequenceNode_upcast(SequenceNode *this) {
	return (Node) { .interface = &INode_SequenceNode, .object = this };
}
