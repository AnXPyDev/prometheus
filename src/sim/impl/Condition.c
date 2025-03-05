#define this ((ConditionNode*)vthis)

void ConditionNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult cond_result = SimResult_NULL;

	SimNode_evaluate(this->condition, context, &cond_result);
	if (cond_result.control) {
		SimResult_forward(&cond_result, out_result);
		goto interrupt;
	}

	if (SimValue_isNull(cond_result.value)) {
		goto branch_false;
	}

	// TOOD improve condition checking
	Size ts = Type_size(cond_result.value.type);
	const char *end = (const char*)cond_result.value.data + ts;
	for (const char *it = cond_result.value.data; it < end; it++) {
		if (*it) goto branch_true;
	}

	branch_false:;
	SimNode_evaluate(this->node_false, context, out_result);
	return;

	branch_true:;
	SimNode_evaluate(this->node_true, context, out_result);
	return;

	interrupt:;
	return;
}

const ISimNode ISimNode_ConditionNode = {
	.evaluate = &ConditionNode_SimNode_evaluate
};

#undef this
