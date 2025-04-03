void Parser_parseControl(ESimControlSignal signal, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult result = ParserResult_NULL;

	Token *tkn = TokenStream_probe(ts);
	switch (tkn->type) {
		case TOKEN_TYPE_END:
		case TOKEN_TYPE_INPUT_END:
		case TOKEN_TYPE_LIST_DELIMITER:
		case TOKEN_TYPE_BRACE_CLOSE:
		case TOKEN_TYPE_SBRACE_CLOSE:
		case TOKEN_TYPE_CBRACE_CLOSE:
			goto null_val;
		default:;
	}

	Parser_parseNode(ts, ctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	null_val:;
	out->node = ControlNode_create(signal, NULL, result.node, ctx->program_alc);
}

#define this ((ControlNode*)vthis)

int ControlNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_ControlNode = {
	.eval_flags = &ControlNode_ParserNode_eval_flags
};
