typedef struct {
	Size size;
	Node nodes[];
} SequenceNode;

SequenceNode *SequenceNode_alloc(Array nodes, Allocator alc) {
	SequenceNode *this = Allocator_malloc(alc, sizeof(SequenceNode) + sizeof(Node) * nodes.size);
	this->size = nodes.size;
	memcpy(this->nodes, nodes.data, sizeof(Node) * nodes.size);
	return this;
}

#define this ((SequenceNode*)vthis)

void SequenceNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "SequenceNode(");
	Node *end = this->nodes + this->size;
	for (Node *it = this->nodes; it < end; it++) {

	}
	OutStream_puts(os, ")");
}

Type SequenceNode_resultType(void *vthis, Contract *alc) {
	// TODO add logic to detect early return
	return Node_resultType(this->nodes[this->size - 1], alc);
}


#undef this

const IPrintable IPrintable_SequenceNode = {
	.print = &SequenceNode_print
};

Printable SequenceNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_SequenceNode, .object = vthis };
}

const INode INode_SequenceNode = {
	.repr_ = &SequenceNode_repr,
	.resultType = &SequenceNode_resultType,
	.simext = ISimNode_SequenceNode,
};

Node SequenceNode_upcast(SequenceNode *this) {
	return (Node) { .interface = &INode_SequenceNode, .object = this };
}
