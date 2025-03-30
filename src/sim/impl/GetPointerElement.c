#define this ((GetPointerElementNode*)vthis)

void GetPointerElementNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);

	if (result.control) {
		SimResult_forward(&result, out_result);
		return;
	}

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("GetPointerElementNode: cannot index null value", vthis, context, out_result);
		return;
	}

	Type RT = Type_strip(result.value.type);

	if (!Type_isPointerType(RT)) {
		SimResult_throwMessage("GetPointerElementNode: value is not ptr", vthis, context, out_result);
		return;
	}

	char *ptr = *(void**)result.value.data;
	ptr += this->offset;
	
	out_result->value = SimValue_create(&ptr, PointerType_create(this->T, context->temp_alc), context->temp_alc);
}

const ISimNode ISimNode_GetPointerElementNode = {
	.evaluate = &GetPointerElementNode_SimNode_evaluate
};

#undef this
