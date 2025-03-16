#define this ((ConditionNode*)vthis)

void ConditionNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult cond_result = SimResult_NULL;

	SimNode_evaluate(this->condition, context, &cond_result);
	if (cond_result.control) {
		switch (cond_result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit;
			default:;
		}

		if (0) handle_emit: {
			if (cond_result.control_target == vthis) {
				out_result->value = cond_result.value;
				return;
			}
		}

		SimResult_forward(&cond_result, out_result);
		return;
	}

	if (SimValue_isNull(cond_result.value)) {
		goto branch_false;
	}

	SimResult result = SimResult_NULL;

	// TOOD improve condition checking
	Size ts = Type_size(cond_result.value.type);
	const char *end = (const char*)cond_result.value.data + ts;
	for (const char *it = cond_result.value.data; it < end; it++) {
		if (*it) goto branch_true;
	}
	goto branch_false;

	if (0) branch_false: {
		SimNode_evaluate(this->node_false, context, &result);
	}

	if (0) branch_true: {
		SimNode_evaluate(this->node_true, context, &result);
	}

	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit_2;
			default:;
		}

		if (0) handle_emit_2: {
			if (result.control_target == vthis) {
				out_result->value = result.value;
				return;
			}
		}

		SimResult_forward(&result, out_result);
		return;
	}

	out_result->value = result.value;
}

const ISimNode ISimNode_ConditionNode = {
	.evaluate = &ConditionNode_SimNode_evaluate
};

#undef this
