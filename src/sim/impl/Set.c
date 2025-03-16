#define this ((SetNode*)vthis)

void SetNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("SetNode: cannot get member", vthis, context, out_result);
		return;
	}

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);
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

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("SetNode: result is null", vthis, context, out_result);
		return;
	}

	Size ts = Type_size(result.value.type);
	memcpy(data, result.value.data, ts);

	return_result:;
	out_result->value = result.value;
}

const ISimNode ISimNode_SetNode = {
	.evaluate = &SetNode_SimNode_evaluate
};

#undef this
