#define this ((SetNode*)vthis)

SimValue SetNode_SimNode_evaluate(void *vthis, SimContext *context) {
	void *data = SimStackFrame_getValue(context->frame, this->member);
	if (!data) {
		fprintf(stderr, "SetNode Error no stack variable");
		return SimValue_NULL;
	}

	SimValue evaluated = SimNode_evaluate(this->value, context);
	if (!SimValue_isNull(evaluated)) {
		fprintf(stderr, "SetNode Error null result");
		return SimValue_NULL;
	}

	Size ts = Type_size(evaluated.type);
	memcpy(data, evaluated.data, ts);

	return evaluated;
}

const ISimNode ISimNode_SetNode = {
	.evaluate = &SetNode_SimNode_evaluate
};

#undef this
