Function *Function_create(MemberList *arguments, Node node, Allocator alc) {
	Function *this = Allocator_malloc(alc, sizeof(Function));
	this->arguments = arguments;
	this->node = node;
	this->type = Type_constcast(FunctionType_create(
		Type_constcast(MemberList_type(arguments, alc)),
		Type_constcast(Node_resultType(node, alc)),
		alc
	));
	return this;
}

Function *Function_create_blank(MemberList *arguments, Type T, Allocator alc) {
	Function *this = Allocator_malloc(alc, sizeof(Function));
	this->arguments = arguments;
	this->type = Type_copy(T, alc);
	this->node = Node_NULL;
	return this;
}

void Function_destroy(Function *this, Allocator alc) {
	Node_destroy(this->node, alc);
	Type_destroy(this->type, alc);
}

#define this ((FunctionValue*)vthis)

void Printable_FunctionValue_print(void *vthis, OutStream os, StringView fmt) {
	if (this->closure) {
		PrintFmt(os, "Function({%p}; @closure={%p})", repr(void*, this->function), repr(void*, this->closure));
	} else {
		PrintFmt(os, "Function({%p})", repr(void*, this->function));
	}
}

const IPrintable IPrintable_FunctionValue = {
	.print = &Printable_FunctionValue_print
};

#undef this

Printable FunctionValue_repr(FunctionValue *this) {
	return (Printable) { .interface = &IPrintable_FunctionValue, .object = this };
}
