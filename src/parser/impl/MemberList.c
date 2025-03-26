MemberList *Parser_parseMemberList(int flags, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
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
