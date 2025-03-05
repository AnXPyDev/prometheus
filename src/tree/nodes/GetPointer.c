typedef struct {
	Node pointer;
} GetPointerNode;

Node GetPointerNode_upcast(GetPointerNode*);
Node GetPointerNode_create(Node pointer, Allocator alc) {
	GetPointerNode *this = Allocator_malloc(alc, sizeof(GetPointerNode));
	this->pointer = pointer;
	return GetPointerNode_upcast(this);
}

#define this ((GetPointerNode*)vthis)

void GetPointerNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "GetPointer({})", Node_repr(this->pointer));
}

Type GetPointerNode_resultType(void *vthis, Allocator alc) {
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

void GetPointerNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->pointer, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_GetPointerNode = {
	.print = &GetPointerNode_print
};

Printable GetPointerNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_GetPointerNode, .object = vthis };
}

INode INode_GetPointerNode = {
	.repr_ = &GetPointerNode_repr,
	.resultType = &GetPointerNode_resultType,
	.destroy = &GetPointerNode_destroy,
};

Node GetPointerNode_upcast(GetPointerNode *this) {
	return (Node) { .interface = &INode_GetPointerNode, .object = this };
}
