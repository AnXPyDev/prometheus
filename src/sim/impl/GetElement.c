#define this ((GetElementNode*)vthis)

void GetElementNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("GetElementNode: cannot get member", vthis, context, out_result);
		goto interrupt;
	}
	
	data = (char*)data + this->offset;

	if (Type_equalPrimitive(Type_strip(this->T), PRIMITIVE_TYPE_ANY)) {
		out_result->value = *(SimValue*)data;
		return;
	}

	out_result->value = SimValue_create_nocopy(data, this->T);

	interrupt:;
	return;
}

const ISimNode ISimNode_GetElementNode = {
	.evaluate = &GetElementNode_SimNode_evaluate
};

#undef this
