#define this ((GetValueElementNode*)vthis)

void GetValueElementNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);

	if (result.control) {
		SimResult_forward(&result, out_result);
		return;
	}

	if (SimValue_isNull(result.value)) {
		SimResult_throwMessage("GetValueElementNode: cannot index null value", vthis, context, out_result);
		return;
	}

	void *data = (char*)result.value.data + this->offset;
	
	if (Type_equalPrimitive(Type_strip(this->T), PRIMITIVE_TYPE_ANY)) {
		out_result->value = *(SimValue*)data;
		return;
	}

	out_result->value = SimValue_create_nocopy(data, this->T);
}

const ISimNode ISimNode_GetValueElementNode = {
	.evaluate = &GetValueElementNode_SimNode_evaluate
};

#undef this
