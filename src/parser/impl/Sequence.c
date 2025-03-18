void Parser_parseSequence(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	Vector nodes; Vector_create(&nodes, sizeof(Node));
	Vector_init(&nodes, 8, ctx->tmp_alc);

	while (true) {
		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			case TOKEN_TYPE_IMPLICIT_END:
				TokenStream_next(ts);
				continue;
			case TOKEN_TYPE_CBRACE_CLOSE:;
			case TOKEN_TYPE_INPUT_END:
				goto handle_end;
			default:;
		}

		ParserResult result = ParserResult_NULL;
		Parser_parseNode(0, ts, ctx, &result);

		if (Parser_checkfwd(&result, out)) return;

		*(Node*)Vector_push(&nodes, ctx->tmp_alc) = result.node;

		if (0) handle_end: break;
	}

	out->node = SequenceNode_create(Vector_array(&nodes), ctx->state->program_alc);
}

#define this ((SequenceNode*)vthis)

int SequenceNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	int result = 0;

	Node *it = this->nodes;
	Node *end = it + this->size;

	for (; it < end; it++) {
		if ((result |= ParserNode_eval_flags(*it, ctx)) & PARSERNODE_EVAL_FLAG_IMPOSSIBLE)
			break;
	}

	return result;
}

#undef this

const IParserNode IParserNode_SequenceNode = {
	.eval_flags = &SequenceNode_ParserNode_eval_flags
};
