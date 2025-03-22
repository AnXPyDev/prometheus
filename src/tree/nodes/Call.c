typedef struct {
	bool dynamic;
	union {
		FunctionValue sta;
		Node dyn;
	} function;
	Size argcount;
	Node arguments[];
} CallNode;

Node CallNode_upcast(CallNode*);
Node CallNode_create(FunctionValue function, Array arguments, Allocator alc) {
	CallNode *this = Allocator_malloc(alc, sizeof(CallNode) + sizeof(Node) * arguments.size);
	this->function.sta = function; 
	this->dynamic = false;
	this->argcount = arguments.size;
	memcpy(this->arguments, arguments.data, sizeof(Node) * this->argcount);
	return CallNode_upcast(this);
}

Node CallNode_createDyn(Node dyn, Array arguments, Allocator alc) {
	CallNode *this = Allocator_malloc(alc, sizeof(CallNode) + sizeof(Node) * arguments.size);
	this->function.dyn = dyn;
	this->dynamic = true;
	this->argcount = arguments.size;
	memcpy(this->arguments, arguments.data, sizeof(Node) * this->argcount);
	return CallNode_upcast(this);
}

#define this ((CallNode*)vthis)

void CallNode_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "Call(");

	if (this->argcount == 0) goto skip_args;

	Node *it = this->arguments;
	Node *end = it + this->argcount;
	for (; it < end - 1; it++) {
		Printable_print(Node_repr(*it), os, BufferView_NULL);
		OutStream_puts(os, ", ");
	}
	Printable_print(Node_repr(*it), os, BufferView_NULL);

	if (0) skip_args: {
		OutStream_puts(os, "()");
	}

	PrintFmt(os, " => {})", 
		this->dynamic ? Node_repr(this->function.dyn) : FunctionValue_repr(&this->function.sta)
	);
}

Type CallNode_resultType(void *vthis, Allocator alc) {
	Type T; 
	if (this->dynamic) {
		T = Node_resultType(this->function.dyn, alc);
	} else {
		T = this->function.sta.function->type;
	}

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

Node CallNode_copy(void *vthis, Allocator alc) {
	CallNode *copy = Allocator_malloc(alc, sizeof(CallNode) + sizeof(Node) * this->argcount);
	copy->function = this->function;
	copy->argcount = this->argcount;

	{
		Node *it = this->arguments;
		Node *end = it + this->argcount;
		Node *dst = copy->arguments;

		for (; it < end; it++) {
			*(dst++) = Node_copy(*it, alc);
		}
	}

	return CallNode_upcast(copy);
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
	.copy = &CallNode_copy
};

Node CallNode_upcast(CallNode *this) {
	return (Node) { .interface = &INode_CallNode, .object = this };
}
