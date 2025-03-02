typedef struct {
	Member *member;
	Node value;
} SetNode;

#define this ((SetNode*)vthis)

void SetNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "SetNode({} = {})", Member_repr(this->member), Node_repr(this->value));
}

Type SetNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->member->type, alc);
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
};

Node SetNode_upcast(SetNode *this) {
	return (Node) { .interface = &INode_SetNode, .object = this };
}
