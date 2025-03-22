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

void Parser_parseFunctionType(TokenStream *ts, ParserContext *ctx, ParserResult *out, Type RT) {
	Vector args; Vector_create(&args, sizeof(Type));
	Vector_init(&args, 4, ctx->tmp_alc);

	while (true) {
		Token *here = TokenStream_probe(ts);

		ParserResult result = ParserResult_NULL;
		result.flags = PARSENODE_FLAG_NO_EXPLICIT_END;
		result.expect = PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE);

		Parser_parseNode(ts, ctx, &result);

		if (Parser_checkfwd(&result, out)) return;
		
		if (!Node_isValueNode(result.node)) {
			Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Non parsetime expression used in function type", out);
			return;
		}

		ValueNode *val = result.node.object;
		
		if (!Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) {
			Parser_throws(ctx, &here->src, PARSER_RESULT_PANIC, "Expected type while parsing function type", out);
			return;
		}

		*(Type*)Vector_push(&args, ctx->tmp_alc) = *(Type*)val->data;
		
		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", out);
				return;
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

	out->node = ValueNode_create(
		PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
		(char*)&FT, ctx->program_alc
	);
}

void Parser_parseFunction(TokenStream *ts, ParserContext *ctx, ParserResult *out, Type RT) {
	MemberList *mlargs = MemberList_create(ctx->state->mla);

	while (true) {
		Token *here = TokenStream_probe(ts);

		ParserResult result = ParserResult_NULL;
		result.flags = PARSENODE_FLAG_NO_EXPLICIT_END | PARSENODE_FLAG_ALLOW_INTRIN | PARSENODE_FLAG_DEFER_DECLARATION;

		Parser_parseNode(ts, ctx, &result);

		if (Parser_checkfwd(&result, out)) return;

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
			return;
		}

		skip:;
		Token *token = TokenStream_probe(ts);
		switch (token->type) {
			default:;
				Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Expected list delimiter", out);
				return;
			case TOKEN_TYPE_BRACE_CLOSE: {
				TokenStream_next(ts);
				goto breakloop;
			}
			case TOKEN_TYPE_LIST_DELIMITER:;
				TokenStream_next(ts);
		}

		if (0) breakloop: break;

	}


	ParserFrame frame; ParserFrame_create(&frame, ctx->frame, mlargs, ctx->tmp_alc);
	ParserContext fctx = *ctx;
	fctx.frame = &frame;

	ParserResult result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END;
	Parser_parseNode(ts, &fctx, &result);

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
		Parser_parseFunction(ts, ctx, out, RT);
	} else {
		Parser_parseFunctionType(ts, ctx, out, RT);
	}
}

void Parser_parseFunction_declaration(TokenStream *ts, ParserContext *ctx, ParserResult *out, ParserIntrin_DECLARATION *info) {
	ParserResult result = ParserResult_NULL;
	Parser_parseFunction_anonymous(ts, ctx, &result, info->type);

	if (Parser_checkfwd(&result, out)) return;

	if (!Node_isValueNode(result.node)) goto err;

	ValueNode *val = (ValueNode*)result.node.object;

	if (Type_isFunctionType(val->T)) goto handle_def;
	else if (Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) goto handle_decl;
	else goto err;

	if (0) handle_def: {
		Type FT = val->T;
		FunctionValue FV = *(FunctionValue*)val->data;

		Member *member = MemberList_add(ctx->frame->memberlist, info->identifier, info->qualifier, FT);

		*(FunctionValue*)ParserFrame_ensureValue(ctx->frame, member) = FV;
	}

	out->node = result.node;

	if (0) handle_decl: {
		Type FT = *(Type*)val->data;
		MemberList_add(ctx->frame->memberlist, info->identifier, info->qualifier, FT);
	}

	out->node = result.node;

	if (0) err: {
		Parser_throws(ctx, NULL, PARSER_RESULT_PANIC, "bad things happend :)", out);
		return;
	}

}
