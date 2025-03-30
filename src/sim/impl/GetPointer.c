#define this ((GetPointerNode*)vthis)

void GetPointerNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->pointer, context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
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

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("GetPointerNode: result is null", vthis, context, out_result);
		return;
	}

	if (!Type_isPointerType(result.value.type)) {
		SimResult_throwMessage("GetPointerNode: result is not pointer", vthis, context, out_result);
		return;
	}

	PointerType *ptr = result.value.type.object;

	void *ptrval = *(void**)result.value.data;
	if (!ptrval) {
		SimResult_throwMessage("GetPointerNode: dereference NULL", vthis, context, out_result);
		return;
	}

	if (Type_equalPrimitive(Type_strip(ptr->T), PRIMITIVE_TYPE_ANY)) {
		out_result->value = *(SimValue*)ptrval;
		return;
	}

	out_result->value = SimValue_create_nocopy(ptrval, ptr->T);
}

const ISimNode ISimNode_GetPointerNode = {
	.evaluate = &GetPointerNode_SimNode_evaluate
};

#undef this
