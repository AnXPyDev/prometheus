void Parser_parseCondition(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult res_cond = ParserResult_NULL;
	Parser_parseNode(PARSENODE_FLAG_NO_EXPLICIT_END, ts, ctx, &res_cond);
	if (Parser_checkfwd(&res_cond, out)) return;
	
	ParserResult res_true = ParserResult_NULL;
	Parser_parseNode(PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END, ts, ctx, &res_true);
	if (Parser_checkfwd(&res_true, out)) return;

	{
		Token *token = TokenStream_probe(ts);

		PrintFmt(g_os_stderr, "TOKENTYPE: {}\n", Token_repr(token));

		switch (token->type) {
			default:;
			case TOKEN_TYPE_END:;
				goto no_else;
			case TOKEN_TYPE_IDENTIFIER:;
		}

		Vector mvps; Vector_create(&mvps, sizeof(MemberValuePair));
		Vector_init(&mvps, 4, ctx->tmp_alc);

		ParserFrame_find(ctx->frame, token->str, &mvps, ctx->tmp_alc);

		MemberValuePair *it = Vector_begin(&mvps);
		MemberValuePair *end = Vector_end(&mvps);

		for (; it < end; it++) {
			if (!it->value) continue;
			if (!Type_equalPrimitive(it->member->type, PRIMITIVE_TYPE_KEYWORD)) continue;

			EParserKeyword kw = *(EParserKeyword*)it->value;

			if (kw != PARSER_KEYWORD_ELSE) continue;

			TokenStream_next(ts);
			goto has_else;
		}

		goto no_else;
	}

	has_else:;
	ParserResult res_false = ParserResult_NULL;
	Parser_parseNode(PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END, ts, ctx, &res_false);
	if (Parser_checkfwd(&res_false, out)) return;

	Node else_node = res_false.node;

	if (0) no_else: {
		else_node = Node_NULL;
	}

	out->node = ConditionNode_create(res_cond.node, res_true.node, else_node, ctx->state->program_alc);
}
