void Parser_parseString(Token *token, ParserContext *ctx, ParserResult *out) {
	if (token->type != TOKEN_TYPE_STRING_LITERAL) {
		Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Parser_parseString: token is not string literal", out);
		return;
	}

	if (!Type_nullOrMatch(out->expect, TYPE_BYTESPAN)) {
		Parser_throws(ctx, &token->src, PARSER_RESULT_ERROR, "Cannot parse string literal as expected type", out);
		return;
	}

	BufferView str = token->str;

	Vector buf; Vector_create(&buf, sizeof(char));
	Vector_init(&buf, str.size, ctx->tmp_alc);

	{
		const char *it = str.data;
		const char *end = it + str.size;

		bool escape = false;

		while (it < end) {
			char c = *(it++);
			if (escape) {
				escape = false;
				switch (c) {
					case 'n': c = '\n'; break;
					case '\\': c = '\\'; break;
					default: {
						char msg[256];
						sprintf(msg, "Unknown escape sequence when parsing string literal: '\\%c'", c);
						Parser_throws(ctx, &token->src, PARSER_RESULT_WARNING, msg, NULL);
						continue;
					}
				}
			} else {
				switch (c) {
					case '\\': {
						escape = true;
						continue;
					}
					default:;
				}
			}

			*(char*)Vector_push(&buf, ctx->tmp_alc) = c;
		}

		if (escape) {
			Parser_throws(ctx, &token->src, PARSER_RESULT_WARNING, "Trailing escape in string literal", NULL);
		}
	}

	Buffer string = Buffer_copy(Vector_bufferView(&buf), ctx->program_alc);
	Vector_destroy(&buf, ctx->tmp_alc);

	char *bs[2] = { string.data, string.data + str.size };
	out->node = ValueNode_create(
		TYPE_BYTESPAN,
		(char*)bs, ctx->program_alc
	);
}
