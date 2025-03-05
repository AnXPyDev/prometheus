#define this ((CastNode*)vthis)

void CastNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	// TODO implement casting
	OutStream_puts(context->state->os_err, "TODO: Implement casting!\n");
}

const ISimNode ISimNode_CastNode = {
	.evaluate = &CastNode_SimNode_evaluate
};

#undef this
