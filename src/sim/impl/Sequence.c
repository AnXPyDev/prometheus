#define this ((SequenceNode*)vthis)

void SequenceNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	Node *end = this->nodes + this->size;
	SimResult result = SimResult_NULL;
	for (Node *it = this->nodes; it < end; it++) {
		SimNode_evaluate(*it, context, &result);
		if (result.control) {
			switch (result.control) {
				case SIM_CONTROL_SIGNAL_EMIT:
					goto handle_emit;
				default:;
			}

			if (0) handle_emit: {
				if (result.control_target == vthis) {
					goto return_result;
				}
			}

			SimResult_forward(&result, out_result);
			return;
		}
	}

	return_result:;
	out_result->value = result.value;
}

const ISimNode ISimNode_SequenceNode = {
	.evaluate = &SequenceNode_SimNode_evaluate
};

#undef this
