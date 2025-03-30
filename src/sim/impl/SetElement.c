#define this ((SetElementNode*)vthis)

void SetElementNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("SetElementNode: cannot get member", vthis, context, out_result);
		return;
	}

	data = (char*)data + this->offset;

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
		SimResult_throwMessage("SetElementNode: result is null", vthis, context, out_result);
		return;
	}

	if (Type_equalPrimitive(Type_strip(this->T), PRIMITIVE_TYPE_ANY)) {
		*(SimValue*)data = result.value;
		return;
	}

	Size tsm = Type_size(this->T);
	Size tsr = Type_size(result.value.type);

	if (tsm != tsr) {
		SimResult_throwMessage("SetElementNode: result type size and element type size do not match", vthis, context, out_result);
		return;
	}

	memcpy(data, result.value.data, tsm);

	return_result:;
	out_result->value = result.value;
}

const ISimNode ISimNode_SetElementNode = {
	.evaluate = &SetElementNode_SimNode_evaluate
};

#undef this
