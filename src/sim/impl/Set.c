#define this ((SetNode*)vthis)

void SetNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("SetNode: cannot get member", vthis, context, out_result);
		goto interrupt;
	}

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		SimResult_forward(&result, out_result);
		goto interrupt;
	}

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("SetNode: result is null", vthis, context, out_result);
		goto interrupt;
	}

	Size ts = Type_size(result.value.type);
	memcpy(data, result.value.data, ts);

	out_result->value = result.value;

	interrupt:;
	return;
}

const ISimNode ISimNode_SetNode = {
	.evaluate = &SetNode_SimNode_evaluate
};

#undef this
