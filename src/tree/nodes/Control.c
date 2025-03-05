typedef struct {
	ESimControlSignal signal;
	void *target;
	Node value;
} ControlNode;

Node ControlNode_upcast(ControlNode*);
Node ControlNode_create(ESimControlSignal signal, void *target, Node value, Allocator alc) {
	ControlNode *this = Allocator_malloc(alc, sizeof(ControlNode));
	this->signal = signal;
	this->target = target;
	this->value = value;
	return ControlNode_upcast(this);
}

#define this ((ControlNode*)vthis)

void ControlNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Control({} -> {%p}; {})", strrepr(ESimControlSignal_PRETTY[this->signal]), repr(void*, this->target), Node_repr(this->value));
}

Type ControlNode_resultType(void *vthis, Allocator alc) {
	return Node_resultType(this->value, alc);
}

void ControlNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->value, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_ControlNode = {
	.print = &ControlNode_print
};

Printable ControlNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_ControlNode, .object = vthis };
}

INode INode_ControlNode = {
	.repr_ = &ControlNode_repr,
	.resultType = &ControlNode_resultType,
	.destroy = &ControlNode_destroy
};

Node ControlNode_upcast(ControlNode *this) {
	return (Node) { .interface = &INode_ControlNode, .object = this };
}
