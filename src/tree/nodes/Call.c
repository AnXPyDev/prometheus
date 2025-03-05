typedef struct {
	Node argument;
	Node function;
} CallNode;

Node CallNode_upcast(CallNode*);
Node CallNode_create(Node argument, Node function, Allocator alc) {
	CallNode *this = Allocator_malloc(alc, sizeof(CallNode));
	this->argument = argument;
	this->function = function;
	return CallNode_upcast(this);
}

#define this ((CallNode*)vthis)

void CallNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "CallNode({} => {})", Node_repr(this->argument), Node_repr(this->function));
}

Type CallNode_resultType(void *vthis, Allocator alc) {
	Type T = Node_resultType(this->function, alc);
	if (!Type_isFunctionType(T)) {
		Type_destroy(T, alc);
		return Type_NULL;
	}

	FunctionType *F = T.object;
	Type result = F->result;
	F->result = Type_NULL;
	FunctionType_destroy(F, alc);

	return result;
}

void CallNode_destroy(void *vthis, Allocator alc) {
	Node_destroy(this->argument, alc);
	Node_destroy(this->function, alc);
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
