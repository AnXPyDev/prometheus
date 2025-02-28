typedef struct {
	SimValue (*evaluate)(void *this, SimContext *context);
} ISimNode;

SimValue Node_SimNode_evaluate(void *vthis, SimContext *state) {
	fprintf(stderr, "placeholder evaluate (%p)\n", vthis);
	return SimValue_NULL;
}

const ISimNode ISimNode_Node = {
	.evaluate = &Node_SimNode_evaluate
};

extern const ISimNode ISimNode_GetNode;
const ISimNode ISimNode_SetNode = ISimNode_Node;
const ISimNode ISimNode_BuiltinNode = ISimNode_Node;
const ISimNode ISimNode_CastNode = ISimNode_Node;
const ISimNode ISimNode_SequenceNode = ISimNode_Node;
const ISimNode ISimNode_ConditionNode = ISimNode_Node;
const ISimNode ISimNode_FrameNode = ISimNode_Node;
