typedef struct {
	Member *member;
	ptrdiff_t offset;
	Type T;
	Node value;
} SetElementNode;

Node SetElementNode_upcast(SetElementNode*);
Node SetElementNode_create(Member *member, ptrdiff_t offset, Type T, Node value, Allocator alc) {
	SetElementNode *this = Allocator_malloc(alc, sizeof(SetElementNode));
	this->member = member;
	this->offset = offset;
	this->T = Type_copy(T, alc);
	this->value = value;
	return SetElementNode_upcast(this);
}

#define this ((SetElementNode*)vthis)

void SetElementNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "SetElement({}[{}].<{}> = {})", Member_repr(this->member), repr(ptrdiff_t, this->offset), Type_repr(this->T), Node_repr(this->value));
}

Type SetElementNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->member->type, alc);
}

void SetElementNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->value, alc);
}

Node SetElementNode_copy(void *vthis, Allocator alc) {
	return SetElementNode_create(this->member, this->offset, this->T, Node_copy(this->value, alc), alc);
}

#undef this

const IPrintable IPrintable_SetElementNode = {
	.print = &SetElementNode_print
};

Printable SetElementNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_SetElementNode, .object = vthis };
}

INode INode_SetElementNode = {
	.repr_ = &SetElementNode_repr,
	.resultType = &SetElementNode_resultType,
	.destroy = &SetElementNode_destroy,
	.copy = &SetElementNode_copy
};

Node SetElementNode_upcast(SetElementNode *this) {
	return (Node) { .interface = &INode_SetElementNode, .object = this };
}

