typedef struct {
	Function *function;
	Size argcount;
	Node arguments[];
} CallNode;

Node CallNode_upcast(CallNode*);
Node CallNode_create(Function *function, Array arguments, Allocator alc) {
	CallNode *this = Allocator_malloc(alc, sizeof(CallNode) + sizeof(Node) * arguments.size);
	this->function = function;
	this->argcount = arguments.size;
	memcpy(this->arguments, arguments.data, sizeof(Node) * this->argcount);
	return CallNode_upcast(this);
}

#define this ((CallNode*)vthis)

void CallNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "Call((");

	Node *it = this->arguments;
	Node *end = it + this->argcount;
	for (; it < end - 1; it++) {
		Printable_print(Node_repr(*it), os, BufferView_NULL);
		OutStream_puts(os, ", ");
	}
	Printable_print(Node_repr(*it), os, BufferView_NULL);

	PrintFmt(os, " => {%p})", repr(void*, this->function));
}

Type CallNode_resultType(void *vthis, Allocator alc) {
	Type T = this->function->type;
	if (!Type_isFunctionType(T)) {
		return Type_NULL;
	}

	FunctionType *F = T.object;
	Type result = F->result;
	F->result = Type_NULL;
	FunctionType_destroy(F, alc);

	return result;
}

void CallNode_destroy(void *vthis, Allocator alc) {
	Node *it = this->arguments;
	Node *end = it + this->argcount;
	for (; it < end; it++) {
		Node_destroy(*it, alc);
	}
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_CallNode = {
	.print = &CallNode_print
};

Printable CallNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_CallNode, .object = vthis };
}

INode INode_CallNode = {
	.repr_ = &CallNode_repr,
	.resultType = &CallNode_resultType,
	.destroy = &CallNode_destroy,
};

Node CallNode_upcast(CallNode *this) {
	return (Node) { .interface = &INode_CallNode, .object = this };
}
