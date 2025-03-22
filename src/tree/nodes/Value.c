typedef struct {
	Type T;
	char data[];
} ValueNode;

Node ValueNode_upcast(ValueNode*);

Node ValueNode_create(Type T, const char *data, Allocator alc) {
	Size ts = Type_size(T);
	ValueNode *this = Allocator_malloc(alc, sizeof(ValueNode) + ts);
	this->T = Type_copy(T, alc);
	memcpy(this->data, data, ts);
	return ValueNode_upcast(this);
}

#define this ((ValueNode*)vthis)

void ValueNode_print(void *vthis, OutStream os, StringView fmt) {
	Size ts = Type_size(this->T);

	Dumper dumper = Dumper_new((BufferView) { .data = this->data, .size = ts }, 0);

	PrintFmt(os, "Value({}; {})", Type_repr(this->T), Dumper_repr(&dumper));
}

Type ValueNode_resultType(void *vthis, Allocator alc) {
	return Type_copy(this->T, alc);
}

void ValueNode_destroy(void *vthis, Allocator alc) {
	Type_destroy(this->T, alc);
	Allocator_free(alc, vthis);
}

Node ValueNode_copy(void *vthis, Allocator alc) {
	return ValueNode_create(this->T, this->data, alc);
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
	.destroy = &ValueNode_destroy,
	.copy = &ValueNode_copy
};

Node ValueNode_upcast(ValueNode *this) {
	return (Node) { .interface = &INode_ValueNode, .object = this };
}

bool Node_isValueNode(Node this) {
	return this.interface == &INode_ValueNode;
}

void *ValueNode_unwrap(ValueNode *this) {
	return this->data;
}
