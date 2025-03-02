#define this ((GetNode*)vthis)

SimValue GetNode_SimNode_evaluate(void *vthis, SimContext *context) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		fprintf(stderr, "GetNode error");
		return SimValue_NULL;
	}

	return SimValue_create(data, this->member->type, context->temp_alc);
}

const ISimNode ISimNode_GetNode = {
	.evaluate = &GetNode_SimNode_evaluate
};

#undef this
