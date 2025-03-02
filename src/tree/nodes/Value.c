typedef struct {
	Type T;
	char data[];
} ValueNode;

ValueNode *ValueNode_create(Allocator alc, Type T, char *data) {
	Size ts = Type_size(T);
	ValueNode *this = Allocator_malloc(alc, sizeof(ValueNode) + ts);
	this->T = Type_copy(T, alc);
	memcpy(this->data, data, ts);
	return this;
}

#define this ((ValueNode*)vthis)

void ValueNode_print(void *vthis, OutStream os, StringView fmt) {
	Size ts = Type_size(this->T);

	Dumper dumper = Dumper_new((BufferView) { .data = this->data, .size = ts }, 0);

	PrintFmt(os, "ValueNode({}; {})", Type_repr(this->T), Dumper_repr(&dumper));
}

Type ValueNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->T, alc);
}

#undef this

const IPrintable IPrintable_ValueNode = {
	.print = &ValueNode_print
};

Printable ValueNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_ValueNode, .object = vthis };
}

INode INode_ValueNode = {
	.repr_ = &ValueNode_repr,
	.resultType = &ValueNode_resultType,
};

Node ValueNode_upcast(ValueNode *this) {
	return (Node) { .interface = &INode_ValueNode, .object = this };
}
