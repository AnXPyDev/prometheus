#define this ((CatchNode*)vthis)

void CatchNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->value, context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_THROW:
				goto handle_throw;
			case SIM_CONTROL_SIGNAL_BREAK:
			case SIM_CONTROL_SIGNAL_RETURN:
			case SIM_CONTROL_SIGNAL_EXIT:
			// TODO handle jump
			default:;
				goto interrupt;
		}

		handle_throw:;
		bool types_equal = Type_equal(this->T, result.value.type);
		if (!result.control_target) {
			if (!types_equal) goto interrupt;
			goto caught;
		}

		if (result.control_target == vthis) {
			if (!types_equal) {
				SimResult_throwMessage("CatchNode: caught type mismatch", vthis, context, out_result);
				goto interrupt;
			}
			goto caught;
		}

		caught:;
	}
		
	out_result->value = result.value;
	return;

	interrupt:;
	SimResult_forward(&result, out_result);
}

const ISimNode ISimNode_CatchNode = {
	.evaluate = &CatchNode_SimNode_evaluate
};

#undef this
