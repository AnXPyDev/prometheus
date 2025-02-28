typedef struct {
	Node condition;
	Node node_true;	
	Node node_false;
} ConditionNode;

#define this ((ConditionNode*)vthis)

void ConditionNode_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "ConditionNode({} ? {} : {})",
		Node_repr(this->condition),
		Node_repr(this->node_true),
		Node_repr(this->node_false)
	);
}

Type ConditionNode_resultType(void *vthis, Contract *alc) {
	// TODO implement union of both cases or enforce same type
	return Node_resultType(this->node_true, alc);
}

#undef this

const IPrintable IPrintable_ConditionNode = {
	.print = &ConditionNode_print
};

Printable ConditionNode_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_ConditionNode, .object = vthis };
}

const INode INode_ConditionNode = {
	.repr_ = &ConditionNode_repr,
	.resultType = &ConditionNode_resultType,

	.simext = ISimNode_ConditionNode,
};

Node ConditionNode_upcast(ConditionNode *this) {
	return (Node) { .interface = &INode_ConditionNode, .object = this };
}
