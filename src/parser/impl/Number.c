void Parser_parseNumber(Token *token, ParserContext *ctx, ParserResult *out) {
	if (token->type != TOKEN_TYPE_NUMERIC_LITERAL) {
		Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Parser_parseNumber: token is not numeric literal", out);
		return;
	}

	if (!Type_nullOrMatch(out->expect, PrimitiveType_upcast(PRIMITIVE_TYPE_INT))) {
		Parser_throws(ctx, &token->src, PARSER_RESULT_ERROR, "Cannot parse numeric literal as expected type", out);
		return;
	}

	BufferView str = token->str;

	char vstr[256];
	memcpy(vstr, str.data, str.size);
	vstr[str.size] = 0;
	int val = atoi(vstr);

	out->node = ValueNode_create(
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
		(char*)&val, ctx->program_alc
	);
}
