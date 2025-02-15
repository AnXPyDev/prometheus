void Parser_tokenize(Parser *parser, ParserInStream *in, Vector *out_tokens) {
	Vector buffer = Vector_new(parser->allocator, 1);
	Vector_init(&buffer, 256);
	Token token;

	ParserChar c;


	discard_token:;
	dispatch: {
		Vector_clear(&buffer);
		token = (Token) {
			.position = in->position,
			.source = in,
		};

		c = ParserInStream_getc(in);
		//fprintf(stderr, "char '%c' (%d)\n", (char)c, (int)c);
		if (ParserInStream_end(in)) goto handle_eof;

		*(char*)(Vector_push(&buffer)) = (char)c;

		int flags = g_Parser_CharFlags[c];
		if (flags & PARSER_CHAR_NEWLINE) goto tkn_implicit_end;
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

			int flags = g_Parser_CharFlags[c];


			if (flags & PARSER_CHAR_IDENTIFIER) {
				*(char*)Vector_push(&buffer) = ParserChar_toChar(c);
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
		token.type = g_Parser_RestrictTypes[c];
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

			int flags = g_Parser_CharFlags[c];
			if (flags & PARSER_CHAR_STRING_ESCAPE) {
				escape = true;
				continue;
			}

			if (flags & PARSER_CHAR_STRING_DELIMITER) break;

			tkn_string_push:;
			escape = false;
			*(ParserChar*)Vector_push(&buffer) = c;
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

			int flags = g_Parser_CharFlags[c];

			if (flags & (PARSER_CHAR_NUMERIC | PARSER_CHAR_NUMERIC_DECIMAL_POINT)) {
				*(ParserChar*)Vector_push(&buffer) = c;
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
			token.str = Buffer_view(Buffer_copy(Vector_bufferView(&buffer), parser->allocator));
		}
		*(Token*)Vector_push(out_tokens) = token;
		goto dispatch;
	}

	handle_eof: {
		token.type = TOKEN_TYPE_INPUT_END;
		*(Token*)Vector_push(out_tokens) = token;
	}

	return;
}
