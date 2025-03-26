typedef struct {
	FunctionType *ft;
	FunctionValue *fv;
} Parser_CallCandidate;

bool Parser_parseArgListCandidate(
	ParserCache *cache, FunctionType *func, TokenStream *ts,
	ParserContext *ctx, Array *out
) {
	bool error = false;

	Type AT = func->argument;
	if (!Type_isTupleType(AT)) abort();

	TupleType *TAT = (TupleType*)AT.object;

	Type *argtype_it = TAT->elements;
	Type *argtype_end = argtype_it + TAT->size;

	ParserResult **results = Allocator_malloc(ctx->tmp_alc, sizeof(ParserResult*) * TAT->size);
	ParserResult **results_end = results + TAT->size;
	ParserResult **rp = results;

	if (TAT->size == 0) goto handle_zero_args;

	while (true) {
		if (argtype_it >= argtype_end) {
			goto handle_error;
		}

		ParserResult result = ParserResult_NULL;
		result.flags = PARSENODE_FLAG_NO_EXPLICIT_END;
		result.expect = *(argtype_it++);

		ParserResult *cached = ParserCache_parseNode(
			cache, ts, ctx, &result
		);

		if (Parser_check(cached)) {
			goto handle_error;
		}

		*(rp++) = cached;

		handle_zero_args:;

		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", NULL);
				goto handle_error;
			case TOKEN_TYPE_BRACE_CLOSE: goto breakloop;
			case TOKEN_TYPE_LIST_DELIMITER:;
				TokenStream_next(ts);
		}

		if (0) breakloop: break;
	}

	if (argtype_it != argtype_end) goto handle_error;
	
	Node *args = Allocator_malloc(ctx->tmp_alc, sizeof(Node) * TAT->size);
	Node *arg = args;

	rp = results;
	while (rp < results_end) {
		ParserResult result = ParserResult_NULL;
 		ParserCache_export(ts, ctx, *(rp++), &result);
		*(arg++) = result.node;
	}

	*out = (Array) { .data = args, .size = TAT->size };

	if (0) handle_error: {
		error = true;
	}

	Allocator_free(ctx->tmp_alc, results);

	return !error;
}


void Parser_parseCall(Array funcs, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserCache cache;
	ParserCache_create(&cache, ctx->tmp_alc);

	Token *here = TokenStream_probe(ts);

	Array args;

	Parser_CallCandidate *it = funcs.data;
	Parser_CallCandidate *end = it + funcs.size;
	for (; it < end; it++) {
		TokenStream_set(ts, here);
		PrintFmt(ctx->dbgstream, "call candidate: {} {}\n", Type_repr(FunctionType_upcast(it->ft)), FunctionValue_repr(it->fv));
		if (Parser_parseArgListCandidate(
			&cache, it->ft, ts, ctx, &args
		)) goto found_arglist;
	}


	Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "No suitable function call", out);
	return;

	found_arglist:;

	out->node = CallNode_create(*it->fv, args, ctx->program_alc);
}

#define this ((CallNode*)vthis)

int CallNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	if (this->dynamic) return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;

	int result = 0;
	{
		Node *it = this->arguments;
		Node *end = it + this->argcount;
		for (; it < end; it++) {
			if ((result |= ParserNode_eval_flags(*it, ctx)) & PARSERNODE_EVAL_FLAG_IMPOSSIBLE)
				return result;
		}
	}

	ParserContext newctx = {
		.state = ctx->state,
		.frame = ctx->state->root_frame,
		.tmp_alc = ctx->tmp_alc	
	};

	result |= ParserNode_eval_flags(this->function.sta.function->node, &newctx);

	return result;
}

#undef this

const IParserNode IParserNode_CallNode = {
	.eval_flags = &CallNode_ParserNode_eval_flags
};
