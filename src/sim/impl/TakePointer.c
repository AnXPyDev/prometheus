#define this ((TakePointerNode*)vthis)

void TakePointerNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		SimResult_throwMessage("TakePointerNode: cannot get member", vthis, context, out_result);
		goto interrupt;
	}

	PointerType pt = {
		.T = this->member->type
	};

	out_result->value = SimValue_create(&data, PointerType_upcast(&pt), context->temp_alc);

	interrupt:;
	return;
}

const ISimNode ISimNode_TakePointerNode = {
	.evaluate = &TakePointerNode_SimNode_evaluate
};

#undef this
