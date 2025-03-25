bool Parser_isFunctionDefinition(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	Token *here = TokenStream_probe(ts);

	ParserResult result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_EXPLICIT_END | PARSENODE_FLAG_ALLOW_INTRIN | PARSENODE_FLAG_DEFER_DECLARATION;

	ParserContext newctx = *ctx;
	newctx.program_alc = ctx->tmp_alc;

	Parser_parseNode(ts, &newctx, &result);
	TokenStream_set(ts, here);

	if (Parser_checkfwd(&result, out)) return false;

	if (Node_isNull(result.node)) {
		return true;
	}


	if (!Node_isValueNode(result.node)) {
		Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Non parsetime expression used in function type", out);
		return false;
	}

	ValueNode *val = result.node.object;

	if (Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) {
		return false;
	} else if (Type_equalPrimitive(val->T, PRIMITIVE_TYPE_PARSER_INTRIN)) {
		ParserIntrin *intrin = *(ParserIntrin**)val->data;
		if (intrin->type == PARSER_INTRIN_DECLARATION) return true;
	}

	Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Invalid value type used in function type", out);
	return false;
}

Type Parser_parseFunctionType(TokenStream *ts, ParserContext *ctx, ParserResult *out, Type RT) {
	Vector args; Vector_create(&args, sizeof(Type));
	Vector_init(&args, 4, ctx->tmp_alc);

	while (true) {
		Token *here = TokenStream_probe(ts);

		ParserResult result = ParserResult_NULL;
		result.flags = PARSENODE_FLAG_NO_EXPLICIT_END;
		result.expect = PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE);

		Parser_parseNode(ts, ctx, &result);

		if (Parser_checkfwd(&result, out)) return Type_NULL;
		
		if (!Node_isValueNode(result.node)) {
			Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Non parsetime expression used in function type", out);
			return Type_NULL;
		}

		ValueNode *val = result.node.object;
		
		if (!Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) {
			Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Expected type while parsing function type", out);
			return Type_NULL;
		}

		*(Type*)Vector_push(&args, ctx->tmp_alc) = *(Type*)val->data;
		
		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", out);
				return Type_NULL;
			case TOKEN_TYPE_BRACE_CLOSE: {
				TokenStream_next(ts);
				goto breakloop;
			}
			case TOKEN_TYPE_LIST_DELIMITER:;
				TokenStream_next(ts);
		}

		if (0) breakloop: break;
	}

	Type *first = Vector_begin(&args);

	if (args.size == 1 && Type_equalPrimitive(*first, PRIMITIVE_TYPE_VOID)) {
		Vector_clear(&args);
	}

	Type FT = FunctionType_create(TupleType_create(Vector_array(&args), ctx->tmp_alc), RT, ctx->program_alc);

	Vector_destroy(&args, ctx->tmp_alc);

	return FT;

}

MemberList *Parser_parseFunctionArgs(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	MemberList *mlargs = MemberList_create(ctx->state->mla);

	while (true) {
		Token *here = TokenStream_probe(ts);

		ParserResult result = ParserResult_NULL;
		result.flags = PARSENODE_FLAG_NO_EXPLICIT_END | PARSENODE_FLAG_ALLOW_INTRIN | PARSENODE_FLAG_DEFER_DECLARATION;

		Parser_parseNode(ts, ctx, &result);

		if (Parser_checkfwd(&result, out)) return NULL;

		if (Node_isNull(result.node)) goto skip;
		
		if (!Node_isValueNode(result.node)) goto err_invalid_type;

		ValueNode *val = result.node.object;
		if (!Type_equalPrimitive(val->T, PRIMITIVE_TYPE_PARSER_INTRIN)) goto err_invalid_type;

		ParserIntrin *intrin = *(ParserIntrin**)val->data;
		if (intrin->type != PARSER_INTRIN_DECLARATION) goto err_invalid_type;

		ParserIntrin_DECLARATION *decl = (ParserIntrin_DECLARATION*)intrin;

		MemberList_add(mlargs, decl->identifier, decl->qualifier, decl->type);

		if (0) err_invalid_type: {
			Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Non parsetime expression used in function type", out);
			return NULL;
		}

		skip:;
		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", out);
				return NULL;
			case TOKEN_TYPE_BRACE_CLOSE: {
				TokenStream_next(ts);
				goto breakloop;
			}
			case TOKEN_TYPE_LIST_DELIMITER:;
				TokenStream_next(ts);
		}

		if (0) breakloop: break;

	}

	return mlargs;
}

void Parser_parseFunctionRoot(TokenStream *ts, ParserContext *ctx, ParserResult *out, MemberList *mlargs) {
	ParserFrame frame; ParserFrame_create(&frame, ctx->frame, mlargs, ctx->tmp_alc);
	ParserContext fctx = *ctx;
	fctx.frame = &frame;

	ParserResult result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END | PARSENODE_FLAG_NO_MARCH;
	Parser_parseNode(ts, &fctx, &result);

	if (Parser_checkfwd(&result, out)) return;

	out->node = result.node;
}

void Parser_parseFunction(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserResult result = ParserResult_NULL;
	MemberList *mlargs = Parser_parseFunctionArgs(ts, ctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	result = ParserResult_NULL;
	Parser_parseFunctionRoot(ts, ctx, out, mlargs);
	if (Parser_checkfwd(&result, out)) return;

	Function *function = Function_create(mlargs, result.node, ctx->program_alc);
	FunctionValue fv = { .function = function, .closure = NULL };
	out->node = ValueNode_create(function->type, (char*)&fv, ctx->program_alc);
}

void Parser_parseFunction_anonymous(
	TokenStream *ts, ParserContext *ctx, ParserResult *out, Type RT
) {
	ParserResult result = ParserResult_NULL;
	bool isdef = Parser_isFunctionDefinition(ts, ctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	if (isdef) {
		Parser_parseFunction(ts, ctx, out);
	} else {
		result = ParserResult_NULL;
		Type FT = Parser_parseFunctionType(ts, ctx, &result, RT);
		if (Parser_checkfwd(&result, out)) return;
		out->node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
			(char*)&FT, ctx->program_alc
		);
	}
}

void Parser_parseFunction_declaration(TokenStream *ts, ParserContext *ctx, ParserResult *out, ParserIntrin_DECLARATION *info) {
	ParserResult result = ParserResult_NULL;
	bool isdef = Parser_isFunctionDefinition(ts, ctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	result = ParserResult_NULL;

	Member *member = NULL;

	if (isdef) {
		MemberList *mlargs = Parser_parseFunctionArgs(ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;
		Type FT = Type_constcast(
			FunctionType_create(
				Type_constcast(MemberList_type(mlargs, ctx->program_alc)),
				Type_constcast(Type_copy(info->type, ctx->program_alc)),
				ctx->program_alc
			)
		);

		Function *func = Function_create_blank(mlargs, FT, ctx->program_alc);

		member = MemberList_add(ctx->frame->memberlist, info->identifier, info->qualifier, FT);

		*(FunctionValue*)ParserFrame_ensureValue(ctx->frame, member) = (FunctionValue) {
			.function = func, .closure = NULL
		};

		result = ParserResult_NULL;
		Parser_parseFunctionRoot(ts, ctx, &result, mlargs);
		if (Parser_checkfwd(&result, out)) return;

		func->node = result.node;

	} else {
		Type FT = Parser_parseFunctionType(ts, ctx, &result, info->type);
		if (Parser_checkfwd(&result, out)) return;

		member = MemberList_add(ctx->frame->memberlist, info->identifier, info->qualifier, FT);
	}

	out->node = ValueNode_create(
		PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER), (char*)&member, ctx->program_alc
	);
}
