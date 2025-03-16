#define this ((SetPointerNode*)vthis)

void SetPointerNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult ptr_result = SimResult_NULL;
	SimNode_evaluate(this->pointer, context, &ptr_result);
	if (ptr_result.control) {
		switch (ptr_result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit;
			default:;
		}

		if (0) handle_emit: {
			if (ptr_result.control_target == vthis) {
				out_result->value = ptr_result.value;
				return;
			}
		}

		SimResult_forward(&ptr_result, out_result);
		return;
	}

	if (SimValue_isNull(ptr_result.value)) {
		SimResult_throwMessage("SetPointerNode: ptr_result is null", vthis, context, out_result);
		return;
	}

	if (!Type_isPointerType(ptr_result.value.type)) {
		SimResult_throwMessage("SetPointerNode: ptr_result is not pointer", vthis, context, out_result);
		return;
	}

	PointerType *ptr = ptr_result.value.type.object;

	void *ptrval = *(void**)ptr_result.value.data;
	if (!ptrval) {
		SimResult_throwMessage("SetPointerNode: write to NULL", vthis, context, out_result);
		return;
	}
	
	SimResult val_result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &val_result);

	if (val_result.control) {
		switch (val_result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit_2;
			default:;
		}

		if (0) handle_emit_2: {
			if (val_result.control_target == vthis) {
				goto return_result;
			}
		}

		SimResult_forward(&val_result, out_result);
		return;
	}

	if (SimValue_isNull(val_result.value)) {
		SimResult_throwMessage("SetPointerNode: val_result is null", vthis, context, out_result);
		return;
	}

	Size sz = Type_size(ptr->T);
	if (sz != Type_size(val_result.value.type)) {
		SimResult_throwMessage("SetPointerNode: types do not match", vthis, context, out_result);
		return;
	}

	memcpy(ptrval, val_result.value.data, sz);

	return_result:;
	out_result->value = val_result.value;
}

const ISimNode ISimNode_SetPointerNode = {
	.evaluate = &SetPointerNode_SimNode_evaluate
};

#undef this
