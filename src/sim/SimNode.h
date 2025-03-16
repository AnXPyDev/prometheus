typedef struct {
	void (*evaluate)(void *this, SimContext *context, SimResult *result);
} ISimNode;

extern const ISimNode ISimNode_GetPointerNode;
extern const ISimNode ISimNode_SetPointerNode;
extern const ISimNode ISimNode_TakePointerNode;
extern const ISimNode ISimNode_GetNode;
extern const ISimNode ISimNode_SetNode;
extern const ISimNode ISimNode_CastNode;
extern const ISimNode ISimNode_SequenceNode;
extern const ISimNode ISimNode_FrameNode;
extern const ISimNode ISimNode_BuiltinNode;
extern const ISimNode ISimNode_ConditionNode;
extern const ISimNode ISimNode_ValueNode;
extern const ISimNode ISimNode_CatchNode;
extern const ISimNode ISimNode_ControlNode;
extern const ISimNode ISimNode_CallNode;
extern const ISimNode ISimNode_LoopNode;
