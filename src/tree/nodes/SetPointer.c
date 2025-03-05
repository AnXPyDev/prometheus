typedef struct {
	Node pointer;
	Node value;
} SetPointerNode;

Node SetPointerNode_upcast(SetPointerNode*);
Node SetPointerNode_create(Node pointer, Node value, Allocator alc) {
	SetPointerNode *this = Allocator_malloc(alc, sizeof(SetPointerNode));
	this->pointer = pointer;
	this->value = value;
	return SetPointerNode_upcast(this);
}

#define this ((SetPointerNode*)vthis)

void SetPointerNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "SetPointer({} <= {})", Node_repr(this->pointer), Node_repr(this->value));
}

Type SetPointerNode_resultType(void *vthis, Allocator alc) {
	Type T = Node_resultType(this->pointer, alc);
	if (!Type_isPointerType(T)) {
		return Type_NULL;
	}

	PointerType *ptr = T.object;

	Type result = ptr->T;
	ptr->T = Type_NULL;

	PointerType_destroy(ptr, alc);
	return result;
}

void SetPointerNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->value, alc);
}

#undef this

const IPrintable IPrintable_SetPointerNode = {
	.print = &SetPointerNode_print
};

Printable SetPointerNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_SetPointerNode, .object = vthis };
}

INode INode_SetPointerNode = {
	.repr_ = &SetPointerNode_repr,
	.resultType = &SetPointerNode_resultType,
	.destroy = &SetPointerNode_destroy,
};

Node SetPointerNode_upcast(SetPointerNode *this) {
	return (Node) { .interface = &INode_SetPointerNode, .object = this };
}
