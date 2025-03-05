typedef struct {
	Type T;
	Node value;
} CastNode;

Node CastNode_upcast(CastNode*);
Node CastNode_create(Type T, Node value, Allocator alc) {
	CastNode *this = Allocator_malloc(alc, sizeof(CastNode));
	this->T = Type_copy(T, alc);
	this->value = value;
	return CastNode_upcast(this);
}

#define this ((CastNode*)vthis)

void CastNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "CastNode({}; {})", Type_repr(this->T), Node_repr(this->value));
}

Type CastNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->T, alc);
}

void CastNode_destroy(void *vthis, Allocator alc) {
	Type_destroy(this->T, alc);
	Node_destroy(this->value, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_CastNode = {
	.print = &CastNode_print
};

Printable CastNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_CastNode, .object = vthis };
}

INode INode_CastNode = {
	.repr_ = &CastNode_repr,
	.resultType = &CastNode_resultType,
	.destroy = &CastNode_destroy
};

Node CastNode_upcast(CastNode *this) {
	return (Node) { .interface = &INode_CastNode, .object = this };
}
