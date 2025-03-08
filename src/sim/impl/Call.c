#define this ((CallNode*)vthis)

void CallNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	MemberList *mlargs = this->function->arguments;
	
	if (this->argcount != mlargs->members.size) {
		SimResult_throwMessage("CallNode: wrong number of args", vthis, context, out_result);
		goto interrupt_1;
	}

	SimMemberListInfo *mlinfo = SimCache_getMemberList(
		&context->state->cache, mlargs
	);

	SimValue *args = Allocator_malloc(context->temp_alc, sizeof(SimValue) * this->argcount);
	
	Node *it = this->arguments;
	Node *end = it + this->argcount;

	SimValue *ap = args;

	Member **mp = (Member**)mlargs->members.data;

	for (; it < end; it++) {
		SimResult result = SimResult_NULL;
		SimNode_evaluate(*it, context, &result);

		if (result.control) {
			SimResult_forward(&result, out_result);
			goto interrupt_1;
		}

		if (SimValue_isNull(result.value)) {
			SimResult_throwMessage("CallNode: arg evaluated to null", vthis, context, out_result);
			goto interrupt_1;
		}

		if (!Type_equal(result.value.type, (*(mp++))->type)) {
			SimResult_throwMessage("CallNode: arg type mismatch", vthis, context, out_result);
			goto interrupt_1;
		}

		*(ap++) = result.value;
	}

	if (false) {
		interrupt_1:;
		return;
	}

	ArenaAllocator temp_alc_;
	ArenaAllocator_create(&temp_alc_, context->state->alc, 2048);
	Allocator temp_alc = ArenaAllocator_upcast(&temp_alc_);

	SimStackFrame *stackframe = SimStackFrame_create(
		context->state->root_frame, this->function->arguments, mlinfo, temp_alc
	);

	{
		SimValue *arg = args;
		for (Size i = 0; i < this->argcount; i++) {
			Size offset = mlinfo->offsets[i];
			memcpy(stackframe->data + offset, (arg++)->data, Type_size(args[i].type));
		}
	}

	SimContext new_context = {
		.frame = stackframe,
		.state = context->state,
		.temp_alc = temp_alc
	};

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this->function->node, &new_context, &result);
	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_BREAK:
				goto handle_break;
			case SIM_CONTROL_SIGNAL_RETURN:
				goto handle_return;
			case SIM_CONTROL_SIGNAL_THROW:
			case SIM_CONTROL_SIGNAL_EXIT:
			// TODO handle jump
			default:;
				goto interrupt_2;
		}

		handle_break:;
		if (result.control_target && result.control_target != vthis) {
			goto interrupt_2;
		}
		
		handle_return:;
	}

	out_result->value = SimValue_copy(result.value, context->temp_alc);
	goto quit;

	interrupt_2:;
	SimResult_copy(&result, out_result, context);

	quit:;
	ArenaAllocator_destroy(&temp_alc_);
}

const ISimNode ISimNode_CallNode = {
	.evaluate = &CallNode_SimNode_evaluate
};

#undef this
