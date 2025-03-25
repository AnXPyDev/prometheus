bool Parser_parseUnaryCandidate(
	ParserCache *cache, Type AT, TokenStream *ts,
	ParserContext *ctx, Node *out
) {
	ParserResult result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END | PARSENODE_FLAG_NO_MARCH;
	result.expect = AT;

	ParserResult *cached = ParserCache_parseNode(
		cache, ts, ctx, &result
	);

	if (Parser_check(cached)) return false;

	ParserCache_export(ts, ctx, cached, &result);

	*out = result.node;

	return true;
}

void Parser_parseCallUnary(Array funcs, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserCache cache;
	ParserCache_create(&cache, ctx->tmp_alc);

	Token *here = TokenStream_probe(ts);

	Node arg;

	Parser_CallCandidate *it = funcs.data;
	Parser_CallCandidate *end = it + funcs.size;
	for (; it < end; it++) {
		Type AT = it->ft->argument;
		if (!Type_isTupleType(AT)) continue;
		TupleType *TAT = AT.object;
		if (TAT->size != 1) continue;

		if (Parser_parseUnaryCandidate(
			&cache, TAT->elements[0], ts, ctx, &arg
		)) goto found_arg;
	}

	Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "No suitable unary opeartor", out);
	return;

	found_arg:;

	out->node = CallNode_create(*it->fv, (Array) { .data = &arg, .size = 1 }, ctx->program_alc);
}

void Parser_parseCallBinary(Array funcs, Node first, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserCache cache;
	ParserCache_create(&cache, ctx->tmp_alc);

	Token *here = TokenStream_probe(ts);

	Type first_T = Node_resultType(first, ctx->tmp_alc);

	#ifdef BUILD_DEBUG
	PrintFmt(ctx->dbgstream, "binary for {} {}\n", Node_repr(first), Type_repr(first_T));
	#endif

	Node args[] = { first, Node_NULL };

	Parser_CallCandidate *it = funcs.data;
	Parser_CallCandidate *end = it + funcs.size;
	for (; it < end; it++) {
		Type AT = it->ft->argument;
		if (!Type_isTupleType(AT)) continue;
		TupleType *TAT = AT.object;
		if (TAT->size != 2) continue;

		if (!Type_match(TAT->elements[0], first_T)) continue;

		if (Parser_parseUnaryCandidate(
			&cache, TAT->elements[1], ts, ctx, &args[1]
		)) goto found_arg;
	}

	Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "No suitable binary opeartor", out);
	return;

	found_arg:;

	out->node = CallNode_create(*it->fv, (Array) { .data = args, .size = 2 }, ctx->program_alc);
}
