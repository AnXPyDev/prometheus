#define this ((CastNode*)vthis)

void CastNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:;
				goto handle_emit;
			default:;
		}

		if (0) handle_emit: {
			if (result.control_target == vthis) {
				out_result->value = result.value;
				return;
			}
		}

		SimResult_forward(&result, out_result);
		return;
	}

	if (Type_equalPrimitive(Type_strip(this->T), PRIMITIVE_TYPE_ANY)) {
		out_result->value = SimValue_create(&result.value, this->T, context->temp_alc);
		return;
	}

	if (Type_size(this->T) != Type_size(result.value.type)) {
		SimResult_throwMessage("CastNode: type size mismatch", vthis, context, out_result);
		return;
	}

	out_result->value = (SimValue) {
		.type = Type_copy(this->T, context->temp_alc), .data = result.value.data
	};
}

const ISimNode ISimNode_CastNode = {
	.evaluate = &CastNode_SimNode_evaluate
};

#undef this
