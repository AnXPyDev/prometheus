#define this ((SequenceNode*)vthis)

SimValue SequenceNode_SimNode_evaluate(void *vthis, SimContext *context) {
	Node *end = this->nodes + this->size;
	SimValue result = SimValue_NULL;
	for (Node *it = this->nodes; it < end; it++) {
		result = SimNode_evaluate(*it, context);
	}

	return result;
}

const ISimNode ISimNode_SequenceNode = {
	.evaluate = &SequenceNode_SimNode_evaluate
};

#undef this
