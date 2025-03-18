typedef struct {
	int (*eval_flags)(void *this, ParserContext *ctx);
} IParserNode;

enum {
	PARSERNODE_EVAL_FLAG_IMPOSSIBLE = 1 << 0,
	PARSERNODE_EVAL_FLAG_STACK = 1 << 1,
};

extern const IParserNode IParserNode_GetPointerNode;
extern const IParserNode IParserNode_SetPointerNode;
extern const IParserNode IParserNode_TakePointerNode;
extern const IParserNode IParserNode_GetNode;
extern const IParserNode IParserNode_SetNode;
extern const IParserNode IParserNode_CastNode;
extern const IParserNode IParserNode_SequenceNode;
extern const IParserNode IParserNode_FrameNode;
extern const IParserNode IParserNode_BuiltinNode;
extern const IParserNode IParserNode_ConditionNode;
extern const IParserNode IParserNode_ValueNode;
extern const IParserNode IParserNode_CatchNode;
extern const IParserNode IParserNode_ControlNode;
extern const IParserNode IParserNode_CallNode;
extern const IParserNode IParserNode_LoopNode;
