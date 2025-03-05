#define this ((GetPointerNode*)vthis)

void GetPointerNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->pointer, context, &result);
	if (result.control) {
		SimResult_forward(&result, out_result);
		goto interrupt;
	}

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("GetPointerNode: result is null", vthis, context, out_result);
		goto interrupt;
	}

	if (!Type_isPointerType(result.value.type)) {
		SimResult_throwMessage("GetPointerNode: result is not pointer", vthis, context, out_result);
		goto interrupt;
	}

	PointerType *ptr = result.value.type.object;

	void *ptrval = *(void**)result.value.data;
	if (!ptrval) {
		SimResult_throwMessage("GetPointerNode: dereference NULL", vthis, context, out_result);
		goto interrupt;
	}

	out_result->value = SimValue_create(ptrval, ptr->T, context->temp_alc);

	interrupt:;
	return;
}

const ISimNode ISimNode_GetPointerNode = {
	.evaluate = &GetPointerNode_SimNode_evaluate
};

#undef this
