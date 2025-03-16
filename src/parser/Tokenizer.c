void Parser_tokenize(ParserInStream *in, Allocator tmp_alc, Vector *out_tokens, Allocator out_alc) {
	Vector_create(out_tokens, sizeof(Token));
	Vector_init(out_tokens, 16, out_alc);

	Vector buffer;
	Vector_create(&buffer, sizeof(char));
	Vector_init(&buffer, 256, tmp_alc);

	#define PUSH_BUFFER(c) *(char*)(Vector_push(&buffer, tmp_alc)) = c
	#define PUSH_TOKEN(t) *(Token*)(Vector_push(out_tokens, out_alc)) = t
	#define CHAR_FLAGS(c) g_Parser_CharFlags[(int)(c)]

	Token token;

	ParserChar c;

	discard_token:;
	dispatch: {
		Vector_clear(&buffer);

		c = ParserInStream_getc(in);

		token = (Token) {
			.src = (ObjectSource) {
				.stream = in,
				.pos = in->position
			},
		};

		//fprintf(stderr, "char '%c' (%d)\n", (char)c, (int)c);
		if (ParserInStream_end(in)) goto handle_eof;

		PUSH_BUFFER(ParserChar_toChar(c));

		int flags = CHAR_FLAGS(ParserChar_toChar(c));
		if (flags & PARSER_CHAR_NEWLINE) goto discard_token;
		if (flags & PARSER_CHAR_WHITESPACE) goto discard_token;
		if (flags & PARSER_CHAR_RESTRICT) goto tkn_restrict;
		if (flags & PARSER_CHAR_STRING_DELIMITER) goto tkn_string;
		if (flags & PARSER_CHAR_NUMERIC) goto tkn_number;

		Vector_pop(&buffer);	
		ParserInStream_ungetc(in, c);
		if (flags & PARSER_CHAR_IDENTIFIER_BEGIN) goto tkn_id;

		fprintf(stderr, "unhandled char '%c' (%d)\n", (char)c, (int)c);
		goto handle_eof;
	}


	tkn_id: {
		token.type = TOKEN_TYPE_IDENTIFIER;

		while (true) {
			c = ParserInStream_getc(in);
			if (ParserInStream_end(in)) goto tkn_id_quit;

			//fprintf(stderr, "tkn_id '%c' (%d)\n", (char)c, (int)c);

			int flags = CHAR_FLAGS(ParserChar_toChar(c));


			if (flags & PARSER_CHAR_IDENTIFIER) {
				PUSH_BUFFER(ParserChar_toChar(c));
				continue;
			}

			goto tkn_id_quit;
		}

		tkn_id_quit:;
		ParserInStream_ungetc(in, c);
		if (buffer.size > 0) goto push_token;
		else goto discard_token;
	}

	tkn_restrict: {
		token.type = g_Parser_RestrictTypes[(int)ParserChar_toChar(c)];
		goto push_token;
	}

	tkn_implicit_end: {
		token.type = TOKEN_TYPE_IMPLICIT_END;
		goto push_token;
	}

	tkn_string: {
		token.type = TOKEN_TYPE_STRING_LITERAL;
		Vector_clear(&buffer);

		bool escape = false;
		while (true) {
			c = ParserInStream_getc(in);
			if (ParserInStream_end(in)) goto tkn_string_eof;
			
			if (escape) goto tkn_string_push;

			int flags = CHAR_FLAGS(ParserChar_toChar(c));
			if (flags & PARSER_CHAR_STRING_ESCAPE) {
				escape = true;
				continue;
			}

			if (flags & PARSER_CHAR_STRING_DELIMITER) break;

			tkn_string_push:;
			escape = false;

			PUSH_BUFFER(ParserChar_toChar(c));
		}

		goto push_token;

		tkn_string_eof:;
		ParserInStream_ungetc(in, c);
	}

	tkn_number: {
		token.type = TOKEN_TYPE_NUMERIC_LITERAL;

		while (true) {
			c = ParserInStream_getc(in);
			if (ParserInStream_end(in)) break;

			int flags = CHAR_FLAGS(ParserChar_toChar(c));

			if (flags & (PARSER_CHAR_NUMERIC | PARSER_CHAR_NUMERIC_DECIMAL_POINT)) {
				PUSH_BUFFER(ParserChar_toChar(c));
				continue;
			}

			if (flags & PARSER_CHAR_NUMERIC_DELIMITER) continue;

			break;
		}

		ParserInStream_ungetc(in, c);
		goto push_token;
	}

	push_token: {
		if (buffer.size > 0) {
			token.str = Buffer_view(Buffer_copy(Vector_bufferView(&buffer), out_alc));
		}
		PUSH_TOKEN(token);
		goto dispatch;
	}

	handle_eof: {
		token.type = TOKEN_TYPE_INPUT_END;
		PUSH_TOKEN(token);
	}

	Vector_destroy(&buffer, tmp_alc);

	return;

	#undef PUSH_BUFFER
	#undef PUSH_TOKEN
	#undef CHAR_FLAGS
}
