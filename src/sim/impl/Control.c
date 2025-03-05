#define this ((ControlNode*)vthis)

void ControlNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;

	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		SimResult_forward(&result, out_result);
		goto interrupt;
	}

	out_result->control = this->signal;
	out_result->control_origin = vthis;
	out_result->control_target = this->target;
	out_result->value = result.value;

	interrupt:;
	return;
}

const ISimNode ISimNode_ControlNode = {
	.evaluate = &ControlNode_SimNode_evaluate
};

#undef this
