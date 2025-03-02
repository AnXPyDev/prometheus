#define this ((CastNode*)vthis)

SimValue CastNode_SimNode_evaluate(void *vthis, SimContext *context) {
	// TODO implement casting
	fprintf(stderr, "TODO cast");
	return SimNode_evaluate(this->value, context);
}

const ISimNode ISimNode_CastNode = {
	.evaluate = &CastNode_SimNode_evaluate
};

#undef this
