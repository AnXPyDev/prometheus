typedef struct {
	Node value;
	ptrdiff_t offset;
	Type T;
} GetValueElementNode;

Node GetValueElementNode_upcast(GetValueElementNode*);
Node GetValueElementNode_create(Node value, ptrdiff_t offset, Type T, Allocator alc) {
	GetValueElementNode *this = Allocator_malloc(alc, sizeof(GetValueElementNode));
	this->value = value;
	this->offset = offset;
	this->T = Type_copy(T, alc);
	return GetValueElementNode_upcast(this);
}

#define this ((GetValueElementNode*)vthis)

void GetValueElementNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "GetValueElement({}[{}].<{}>)", 
		Node_repr(this->value),
		repr(ptrdiff_t, this->offset),
		Type_repr(this->T)
	);
}

Type GetValueElementNode_resultType(void *vthis, Allocator alc) {
	return Type_constcast(this->T);
}

void GetValueElementNode_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

Node GetValueElementNode_copy(void *vthis, Allocator alc) {
	return GetValueElementNode_create(Node_copy(this->value, alc), this->offset, this->T, alc);
}

#undef this

const IPrintable IPrintable_GetValueElementNode = {
	.print = &GetValueElementNode_print
};

Printable GetValueElementNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetValueElementNode, .object = vthis };
}

INode INode_GetValueElementNode = {
	.repr_ = &GetValueElementNode_repr,
	.resultType = &GetValueElementNode_resultType,
	.destroy = &GetValueElementNode_destroy,
	.copy = &GetValueElementNode_copy

	// simext initialized at runtime
};

Node GetValueElementNode_upcast(GetValueElementNode *this) {
	return (Node) { .interface = &INode_GetValueElementNode, .object = this };
}
