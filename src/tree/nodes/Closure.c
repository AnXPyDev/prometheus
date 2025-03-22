typedef struct {
	MemberList *memberlist;
	Node setup;
	Function *func;
   void *values[];
} ClosureNode;

Node ClosureNode_upcast(ClosureNode*);

Node ClosureNode_create(MemberList *memberlist, Node setup, Function *func, Allocator alc) {
	ClosureNode *this = Allocator_malloc(alc, sizeof(ClosureNode) + sizeof(void*) * memberlist->members.size);
	this->memberlist = MemberList_constcopy(memberlist);
	this->setup = setup;
	this->func = func;
	return ClosureNode_upcast(this);
}


#define this ((ClosureNode*)vthis)

void ClosureNode_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, "Closure([");
    MemberList_print(this->memberlist, os, BufferView_NULL);
    PrintFmt(os, "]; {})", Node_repr(this->setup));
}

const IPrintable IPrintable_ClosureNode = {
    .print = &ClosureNode_print
};

Printable ClosureNode_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_ClosureNode, .object = vthis };
}

Type ClosureNode_resultType(void *vthis, Allocator alc) {
    return this->func->type;
}

void ClosureNode_destroy(void *vthis, Allocator alc) {
    MemberList_destroy(this->memberlist);
    Node_destroy(this->setup, alc);
    Allocator_free(alc, vthis);
}

Node ClosureNode_copy(void *vthis, Allocator alc) {
    Size msize = this->memberlist->members.size;
    ClosureNode *copy = Allocator_calloc(alc, sizeof(ClosureNode));
    copy->setup = Node_copy(this->setup, alc);
    copy->memberlist = MemberList_constcopy(this->memberlist);
    memcpy(copy->values, this->values, sizeof(void*) * msize);
    return ClosureNode_upcast(this);
}

#undef this

INode INode_ClosureNode = {
    .repr_ = &ClosureNode_repr,
    .resultType = &ClosureNode_resultType,
    .destroy = &ClosureNode_destroy,
    .copy = &ClosureNode_copy
};

Node ClosureNode_upcast(ClosureNode *this) {
    return (Node) { .interface = &INode_ClosureNode, .object = this };
}

