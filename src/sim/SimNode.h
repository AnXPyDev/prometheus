typedef struct {
	SimValue (*evaluate)(void *this, SimContext *context);
} ISimNode;

extern const ISimNode ISimNode_GetNode;
extern const ISimNode ISimNode_SetNode;
extern const ISimNode ISimNode_CastNode;
extern const ISimNode ISimNode_SequenceNode;
extern const ISimNode ISimNode_FrameNode;
extern const ISimNode ISimNode_BuiltinNode;
extern const ISimNode ISimNode_ConditionNode;
extern const ISimNode ISimNode_ValueNode;
