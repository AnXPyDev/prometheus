typedef struct {
	Type T;
	Node value;
} CatchNode;

Node CatchNode_upcast(CatchNode*);
Node CatchNode_create(Type T, Node value, Allocator alc) {
	CatchNode *this = Allocator_malloc(alc, sizeof(CatchNode));
	this->T = Type_copy(T, alc);
	this->value = value;
	return CatchNode_upcast(this);
}

#define this ((CatchNode*)vthis)

void CatchNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "CatchNode({}; {})", Type_repr(this->T), Node_repr(this->value));
}

Type CatchNode_resultType(void *vthis, Allocator alc) {
	Type RT = Node_resultType(this->value, alc);
	if (Type_equal(this->T, RT)) {
		return RT;
	}

	return UnionType_create((Array) { .data = (Type[]) { this->T, RT }, .size = 2 }, alc);
}

void CatchNode_destroy(void *vthis, Allocator alc) {
	Type_destroy(this->T, alc);
	Node_destroy(this->value, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_CatchNode = {
	.print = &CatchNode_print
};

Printable CatchNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_CatchNode, .object = vthis };
}

INode INode_CatchNode = {
	.repr_ = &CatchNode_repr,
	.resultType = &CatchNode_resultType,
	.destroy = &CatchNode_destroy
};

Node CatchNode_upcast(CatchNode *this) {
	return (Node) { .interface = &INode_CatchNode, .object = this };
}
