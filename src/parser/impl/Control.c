void Parser_parseControl(ESimControlSignal signal, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult result = ParserResult_NULL;
	Parser_parseNode(0, ts, ctx, &result);

	if (Parser_checkfwd(&result, out)) return;

	out->node = ControlNode_create(signal, NULL, result.node, ctx->state->program_alc);
}
