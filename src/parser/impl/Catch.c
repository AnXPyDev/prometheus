#define this ((CatchNode*)vthis)

int CatchNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_CatchNode = {
	.eval_flags = &CatchNode_ParserNode_eval_flags
};
