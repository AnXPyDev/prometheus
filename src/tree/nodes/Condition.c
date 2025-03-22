typedef struct {
	Node condition;
	Node node_true;	
	Node node_false;
} ConditionNode;

Node ConditionNode_upcast(ConditionNode*);
Node ConditionNode_create(Node condition, Node branch_true, Node branch_false, Allocator alc) {
	ConditionNode *this = Allocator_malloc(alc, sizeof(ConditionNode));
	this->condition = condition;
	this->node_true = branch_true;
	this->node_false = branch_false;
	return ConditionNode_upcast(this);
}

#define this ((ConditionNode*)vthis)

void ConditionNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Condition({} ? {} : {})",
		Node_repr(this->condition),
		Node_repr(this->node_true),
		Node_repr(this->node_false)
	);
}

Type ConditionNode_resultType(void *vthis, Allocator alc) {
	Type T1 = Node_resultType(this->node_true, alc);
	Type T2 = Node_resultType(this->node_false, alc);
	if (Type_equal(T1, T2)) {
		return T1;
	}

	return UnionType_create_move((Array) { .data = (Type[]) { T1, T2 }, .size = 2 }, alc);
}

void ConditionNode_destroy(void *vthis, Allocator alc)  {
	Node_destroy(this->condition, alc);
	Node_destroy(this->node_true, alc);
	Node_destroy(this->node_false, alc);
	Allocator_free(alc, vthis);
}

Node ConditionNode_copy(void *vthis, Allocator alc) {
	return ConditionNode_create(
		Node_copy(this->condition, alc),
		Node_copy(this->node_true, alc),
		Node_copy(this->node_false, alc),
		alc
	);
}

#undef this

const IPrintable IPrintable_ConditionNode = {
	.print = &ConditionNode_print
};

Printable ConditionNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_ConditionNode, .object = vthis };
}

INode INode_ConditionNode = {
	.repr_ = &ConditionNode_repr,
	.resultType = &ConditionNode_resultType,
	.destroy = &ConditionNode_destroy,
	.copy = &ConditionNode_copy
};

Node ConditionNode_upcast(ConditionNode *this) {
	return (Node) { .interface = &INode_ConditionNode, .object = this };
}
