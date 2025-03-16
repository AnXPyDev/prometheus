void Parser_parseLoop(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult result = ParserResult_NULL;
	Parser_parseNode(PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END, ts, ctx, &result);
	
	if (Parser_checkfwd(&result, out)) return;

	out->node = LoopNode_create(result.node, ctx->state->program_alc);
}
