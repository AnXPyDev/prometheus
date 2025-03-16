void Parser_parseCall(Array funcs, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	Vector args; Vector_create(&args, sizeof(Node));
	Vector_init(&args, 8, ctx->tmp_alc);

	Token *here = TokenStream_probe(ts);

	while (true) {
		ParserResult result = ParserResult_NULL;
		Parser_parseNode(PARSENODE_FLAG_NO_EXPLICIT_END, ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;
		*(Node*)Vector_push(&args, ctx->tmp_alc) = result.node;

		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throw(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", out);
				return;
			case TOKEN_TYPE_BRACE_CLOSE: goto breakloop;
			case TOKEN_TYPE_LIST_DELIMITER:;
				TokenStream_next(ts);
		}

		if (0) breakloop: break;
	}

	TupleType *AT = TupleType_alloc(args.size, ctx->tmp_alc);
	AT->size = args.size;

	{
		Type *dst = AT->elements;

		Node *it = Vector_begin(&args);
		Node *end = Vector_end(&args);

		for (; it < end; it++) {
			*(dst++) = Node_resultType(*it, ctx->tmp_alc);
		}
	}

	Function *function = NULL;

	{
		Function **it = funcs.data;
		Function **end = it + funcs.size;
		for (; it < end; it++) {
			FunctionType *ft = (*it)->type.object;
			if (Type_match(ft->argument, TupleType_upcast(AT))) {
				function = *it;
				break;
			}
		}
	}

	if (!function) {
		Parser_throw(ctx, &here->src, PARSER_RESULT_PANIC, "No candidate matches call signature", out);
		return;
	}

	out->node = CallNode_create(function, Vector_array(&args), ctx->state->program_alc);
}
