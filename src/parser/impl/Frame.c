void Parser_parseFrame(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserFrame frame;
	ParserFrame_create(&frame, ctx->frame, MemberList_create(ctx->state->program_alc), ctx->tmp_alc);

	ParserContext nctx = {
		.frame = &frame,
		.state = ctx->state,
		.tmp_alc = ctx->tmp_alc
	};

	ParserResult result = ParserResult_NULL;

	Parser_parseSequence(ts, &nctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	out->node = FrameNode_create(frame.memberlist, result.node, ctx->state->program_alc);
}
