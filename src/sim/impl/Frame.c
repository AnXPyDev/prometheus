#define this ((FrameNode*)vthis)

SimValue FrameNode_SimNode_evaluate(void *vthis, SimContext *context) {
	ArenaAllocator temp_alc_;
	ArenaAllocator_create(&temp_alc_, context->state->alc, 2048);
	Allocator temp_alc = ArenaAllocator_upcast(&temp_alc_);
	SimStackFrame *stackframe = SimStackFrame_create(vthis, temp_alc);
	stackframe->parent = context->frame;

	SimContext new_context = {
		.frame = stackframe,
		.state = context->state,
		.temp_alc = temp_alc
	};

	SimValue result = SimNode_evaluate(this->root, &new_context);

	Size ts = Type_size(result.type);
	
	void *data = Allocator_malloc(context->temp_alc, ts);
	memcpy(data, result.data, ts);

	ArenaAllocator_destroy(&temp_alc_);

	return (SimValue) {
		.data = data,
		.type = Type_copy(result.type, context->temp_alc)
	};
}

const ISimNode ISimNode_FrameNode = {
	.evaluate = &FrameNode_SimNode_evaluate
};

#undef this
