#define this ((ClosureNode*)vthis)

int ClosureNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
}

#undef this

const IParserNode IParserNode_ClosureNode = {
	.eval_flags = &ClosureNode_ParserNode_eval_flags
};
