#define this ((FrameNode*)vthis)

void FrameNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	ArenaAllocator temp_alc_;
	ArenaAllocator_create(&temp_alc_, context->state->alc, 2048);
	Allocator temp_alc = ArenaAllocator_upcast(&temp_alc_);

	SimMemberListInfo *mlinfo = SimCache_getMemberList(&context->state->cache, this->memberlist);

	SimStackFrame *stackframe = SimStackFrame_create(context->frame, this->memberlist, mlinfo, temp_alc);

	SimContext new_context = {
		.frame = stackframe,
		.state = context->state,
		.temp_alc = temp_alc
	};

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->root, &new_context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_BREAK:
				goto handle_break;
			case SIM_CONTROL_SIGNAL_RETURN:
			case SIM_CONTROL_SIGNAL_THROW:
			case SIM_CONTROL_SIGNAL_EXIT:
			// TODO handle jump
			default:;
				goto interrupt;
		}

		handle_break:;
		if (result.control_target && result.control_target != vthis) {
			goto interrupt;
		}
	}

	out_result->value = SimValue_copy(result.value, context->temp_alc);
	goto quit;

	interrupt:;
	SimResult_copy(&result, out_result, context);

	quit:;
	ArenaAllocator_destroy(&temp_alc_);
}

const ISimNode ISimNode_FrameNode = {
	.evaluate = &FrameNode_SimNode_evaluate
};

#undef this
