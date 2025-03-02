#define this ((ConditionNode*)vthis)

SimValue ConditionNode_SimNode_evaluate(void *vthis, SimContext *context) {
	SimValue cond_result = SimNode_evaluate(this->condition, context);
	if (SimValue_isNull(cond_result)) {
		goto branch_false;
	}

	// TOOD improve condition checking
	Size ts = Type_size(cond_result.type);
	const char *end = (const char*)cond_result.data + ts;
	for (const char *it = cond_result.data; it < end; it++) {
		if (*it) goto branch_true;
	}

	branch_false:;
	return SimNode_evaluate(this->node_false, context);

	branch_true:;
	return SimNode_evaluate(this->node_true, context);
}

const ISimNode ISimNode_ConditionNode = {
	.evaluate = &ConditionNode_SimNode_evaluate
};

#undef this
