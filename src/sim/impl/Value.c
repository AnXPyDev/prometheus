#define this ((ValueNode*)vthis)

void ValueNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	out_result->value = SimValue_create(this->data, this->T, context->temp_alc);
}

const ISimNode ISimNode_ValueNode = {
	.evaluate = &ValueNode_SimNode_evaluate
};

#undef this
