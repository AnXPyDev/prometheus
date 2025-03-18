#define this ((GetNode*)vthis)

void GetNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("GetNode: cannot get member", vthis, context, out_result);
		goto interrupt;
	}

	if (Type_equalPrimitive(Type_strip(this->member->type), PRIMITIVE_TYPE_ANY)) {
		out_result->value = *(SimValue*)data;
		return;
	}

	out_result->value = SimValue_create(data, this->member->type, context->temp_alc);

	interrupt:;
	return;
}

const ISimNode ISimNode_GetNode = {
	.evaluate = &GetNode_SimNode_evaluate
};

#undef this
