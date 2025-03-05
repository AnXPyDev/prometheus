#define this ((SetPointerNode*)vthis)

void SetPointerNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult ptr_result = SimResult_NULL;
	SimNode_evaluate(this->pointer, context, &ptr_result);
	if (ptr_result.control) {
		SimResult_forward(&ptr_result, out_result);
		goto interrupt;
	}

	if (SimValue_isNull(ptr_result.value)) {
		SimResult_throwMessage("SetPointerNode: ptr_result is null", vthis, context, out_result);
		goto interrupt;
	}

	if (!Type_isPointerType(ptr_result.value.type)) {
		SimResult_throwMessage("SetPointerNode: ptr_result is not pointer", vthis, context, out_result);
		goto interrupt;
	}

	PointerType *ptr = ptr_result.value.type.object;

	void *ptrval = *(void**)ptr_result.value.data;
	if (!ptrval) {
		SimResult_throwMessage("SetPointerNode: write to NULL", vthis, context, out_result);
		goto interrupt;
	}
	
	SimResult val_result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &val_result);

	if (val_result.control) {
		SimResult_forward(&val_result, out_result);
		goto interrupt;
	}

	if (SimValue_isNull(val_result.value)) {
		SimResult_throwMessage("SetPointerNode: val_result is null", vthis, context, out_result);
		goto interrupt;
	}

	Size sz = Type_size(ptr->T);
	if (sz != Type_size(val_result.value.type)) {
		SimResult_throwMessage("SetPointerNode: types do not match", vthis, context, out_result);
		goto interrupt;
	}

	memcpy(ptrval, val_result.value.data, sz);

	out_result->value = val_result.value;

	interrupt:;
	return;
}

const ISimNode ISimNode_SetPointerNode = {
	.evaluate = &SetPointerNode_SimNode_evaluate
};

#undef this
