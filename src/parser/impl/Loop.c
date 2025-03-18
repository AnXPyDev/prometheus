void Parser_parseLoop(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult result = ParserResult_NULL;
	Parser_parseNode(PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END, ts, ctx, &result);
	
	if (Parser_checkfwd(&result, out)) return;

	out->node = LoopNode_create(result.node, ctx->state->program_alc);
}

#define this ((LoopNode*)vthis)

int LoopNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_LoopNode = {
	.eval_flags = &LoopNode_ParserNode_eval_flags
};
