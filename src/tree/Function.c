typedef struct {
	MemberList *arguments;
	Node node;
	Type type;
} Function;

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

void Function_destroy(Function *this, Allocator alc) {
	Node_destroy(this->node, alc);
	Type_destroy(this->type, alc);
}
