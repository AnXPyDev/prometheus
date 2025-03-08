#define this ((CastNode*)vthis)

void CastNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		SimResult_forward(&result, out_result);
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
