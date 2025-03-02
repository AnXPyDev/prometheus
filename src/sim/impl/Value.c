#define this ((ValueNode*)vthis)

SimValue ValueNode_SimNode_evaluate(void *vthis, SimContext *context) {
	SimValue val = SimValue_create(this->data, this->T, context->temp_alc);
	return val;
}

const ISimNode ISimNode_ValueNode = {
	.evaluate = &ValueNode_SimNode_evaluate
};

#undef this
