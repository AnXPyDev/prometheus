#define this ((FrameNode*)vthis)

void FrameNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	ArenaAllocator temp_alc_;
	ArenaAllocator_create(&temp_alc_, context->state->alc, 2048);
	Allocator temp_alc = ArenaAllocator_upcast(&temp_alc_);

	SimMemberListInfo *mlinfo = SimCache_getMemberList(&context->state->cache, this->memberlist);

	SimStackFrame *stackframe = SimStackFrame_create(context->frame, this->memberlist, mlinfo, temp_alc, temp_alc);

	/* copy defaults */ {
		SimMemberInfo *info = mlinfo->info;
		void **it = this->values;
		void **end = it + this->memberlist->members.size;
		for (; it < end; it++) {
			if (*it) {
				memcpy(stackframe->data + info->offset, *it, info->type_size);
			}
			info++;
		}
	}
	
	SimContext new_context = {
		.frame = stackframe,
		.state = context->state,
		.temp_alc = temp_alc
	};

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->root, &new_context, &result);
	SimStackFrame_evaluateDeferred(stackframe, &new_context, &result);

	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit;
			default:;
		}


		if (0) handle_emit: {
			if (!result.control_target || result.control_target == vthis) {
				goto return_result;	
			}
		}

		SimResult_copy(&result, out_result, context);
		goto quit;
	}

	return_result:;
	out_result->value = SimValue_copy(result.value, context->temp_alc);

	SimStackFrame_destroy(stackframe, temp_alc);

	quit:;
	ArenaAllocator_destroy(&temp_alc_);
}

const ISimNode ISimNode_FrameNode = {
	.evaluate = &FrameNode_SimNode_evaluate
};

#undef this
