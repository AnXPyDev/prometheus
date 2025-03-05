#define this ((SequenceNode*)vthis)

void SequenceNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	Node *end = this->nodes + this->size;
	for (Node *it = this->nodes; it < end; it++) {
		SimNode_evaluate(*it, context, &result);
		if (result.control) {
			SimResult_forward(&result, out_result);
			goto interrupt;
		}
	}

	out_result->value = result.value;
	return;

	interrupt:;
	return;
}

const ISimNode ISimNode_SequenceNode = {
	.evaluate = &SequenceNode_SimNode_evaluate
};

#undef this
