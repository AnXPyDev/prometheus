typedef struct {
	Member *member;
	Node value;
} SetNode;

Node SetNode_upcast(SetNode*);
Node SetNode_create(Member *member, Node value, Allocator alc) {
	SetNode *this = Allocator_malloc(alc, sizeof(SetNode));
	this->value = value;
	this->member = member;
	return SetNode_upcast(this);
}

#define this ((SetNode*)vthis)

void SetNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Set({} = {})", Member_repr(this->member), Node_repr(this->value));
}

Type SetNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->member->type, alc);
}

void SetNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->value, alc);
}

Node SetNode_copy(void *vthis, Allocator alc) {
	return SetNode_create(this->member, Node_copy(this->value, alc), alc);
}

#undef this

const IPrintable IPrintable_SetNode = {
	.print = &SetNode_print
};

Printable SetNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_SetNode, .object = vthis };
}

INode INode_SetNode = {
	.repr_ = &SetNode_repr,
	.resultType = &SetNode_resultType,
	.destroy = &SetNode_destroy,
	.copy = &SetNode_copy
};

Node SetNode_upcast(SetNode *this) {
	return (Node) { .interface = &INode_SetNode, .object = this };
}
