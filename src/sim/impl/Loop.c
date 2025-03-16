#define this ((LoopNode*)vthis)

void LoopNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result;
	while (true) {
		result = SimResult_NULL;
		SimNode_evaluate(this->value, context, &result);
		if (result.control) {
			switch (result.control) {
				case SIM_CONTROL_SIGNAL_CONTINUE:
					goto handle_continue;
				case SIM_CONTROL_SIGNAL_BREAK:
					goto handle_break;
				case SIM_CONTROL_SIGNAL_EMIT:
					goto handle_emit;
				default:;
			}

			if (0) handle_break: {
				if (!result.control_target || result.control_target == vthis) {
					break;
				}
			}

			if (0) handle_continue: continue;

			if (0) handle_emit: {
				if (result.control_target == vthis) {
					break;
				}
			}

			SimResult_forward(&result, out_result);
			return;
		}
	}
	
	out_result->value = result.value;
}

const ISimNode ISimNode_LoopNode = {
	.evaluate = &LoopNode_SimNode_evaluate
};

#undef this
