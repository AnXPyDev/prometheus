#define this ((ControlNode*)vthis)

void ControlNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;

	switch (this->signal) {
		case SIM_CONTROL_SIGNAL_DEFER:;
			goto handle_defer;
		default:;
	}

	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:;
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

	out_result->control = this->signal;
	out_result->control_origin = vthis;
	out_result->control_target = this->target;
	out_result->value = result.value;

	if (0) handle_defer: {
		*(Node*)Vector_push(&context->frame->deferred, context->frame->deferred_alc) = this->value;
	}
}

const ISimNode ISimNode_ControlNode = {
	.evaluate = &ControlNode_SimNode_evaluate
};

#undef this
