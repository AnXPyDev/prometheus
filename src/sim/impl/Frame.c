#define this ((FrameNode*)vthis)

void FrameNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	ArenaAllocator temp_alc_;
	ArenaAllocator_create(&temp_alc_, context->state->alc, 2048);
	Allocator temp_alc = ArenaAllocator_upcast(&temp_alc_);

	SimStackFrame *stackframe = SimStackFrame_create((Array) {
		.data = (void*)&(MemberList*) { &this->ml }, .size = 1
	}, &context->state->mlCache, temp_alc);

	stackframe->parent = context->frame;

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
	return;

	interrupt:;
	SimResult_copy(&result, out_result, context);
}

const ISimNode ISimNode_FrameNode = {
	.evaluate = &FrameNode_SimNode_evaluate
};

#undef this
