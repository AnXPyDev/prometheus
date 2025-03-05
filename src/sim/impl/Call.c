#define this ((CallNode*)vthis)

void CallNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	interrupt:;
	return;
}

const ISimNode ISimNode_CallNode = {
	.evaluate = &CallNode_SimNode_evaluate
};

#undef this
