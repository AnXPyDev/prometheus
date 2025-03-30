typedef struct {
	Node value;
	ptrdiff_t offset;
	Type T;
} GetPointerElementNode;

Node GetPointerElementNode_upcast(GetPointerElementNode*);
Node GetPointerElementNode_create(Node value, ptrdiff_t offset, Type T, Allocator alc) {
	GetPointerElementNode *this = Allocator_malloc(alc, sizeof(GetPointerElementNode));
	this->value = value;
	this->offset = offset;
	this->T = Type_copy(T, alc);
	return GetPointerElementNode_upcast(this);
}

#define this ((GetPointerElementNode*)vthis)

void GetPointerElementNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "GetPointerElement({}[{}].<{}>)", 
		Node_repr(this->value),
		repr(ptrdiff_t, this->offset),
		Type_repr(this->T)
	);
}

Type GetPointerElementNode_resultType(void *vthis, Allocator alc) {
	return PointerType_create(Type_constcast(this->T), alc);
}

void GetPointerElementNode_destroy(void *vthis, Allocator alc) {
	Allocator_free(alc, vthis);
}

Node GetPointerElementNode_copy(void *vthis, Allocator alc) {
	return GetPointerElementNode_create(Node_copy(this->value, alc), this->offset, this->T, alc);
}

#undef this

const IPrintable IPrintable_GetPointerElementNode = {
	.print = &GetPointerElementNode_print
};

Printable GetPointerElementNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetPointerElementNode, .object = vthis };
}

INode INode_GetPointerElementNode = {
	.repr_ = &GetPointerElementNode_repr,
	.resultType = &GetPointerElementNode_resultType,
	.destroy = &GetPointerElementNode_destroy,
	.copy = &GetPointerElementNode_copy

	// simext initialized at runtime
};

Node GetPointerElementNode_upcast(GetPointerElementNode *this) {
	return (Node) { .interface = &INode_GetPointerElementNode, .object = this };
}

