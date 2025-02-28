SimValue GetNode_SimNode_evaluate(void *vthis, SimContext *context) {
	return SimValue_NULL;
}

const ISimNode ISimNode_GetNode = {
	.evaluate = &GetNode_SimNode_evaluate
};
