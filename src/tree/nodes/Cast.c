typedef struct {
	Type T;
	Node value;
} CastNode;

#define this ((CastNode*)vthis)

void CastNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "CastNode({} : {})", Node_repr(this->value), Type_repr(this->T));
}

Type CastNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->T, alc);
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
};

Node CastNode_upcast(CastNode *this) {
	return (Node) { .interface = &INode_CastNode, .object = this };
}
