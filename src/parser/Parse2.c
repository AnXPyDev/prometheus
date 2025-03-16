typedef enum {
	PARSER_STATE_NONE = 0,
	PARSER_STATE_DSP_IDENTIFIER,
	PARSER_STATE_DSP_NODE,
	PARSER_STATE_DSP_CONTROL
} EParserState;

typedef union {
	Node node;
	StringView id;	
} ParserRegister;

void Parser_parseNumber(TokenStream *ts, Type expect, ParserContext *context, ParserResult *out_result) {
	// TODO other types than int
	Token *tkn = TokenStream_next(ts);
	int *val = Allocator_malloc(context->state->program_alc, sizeof(int));

	// TODO custom parser
	char vstr[256];
	memcpy(vstr, tkn->str.data, tkn->str.size);
	vstr[tkn->str.size] = 0;
	*val = atoi(vstr);
	out_result->node = ValueNode_create(TYPE_INT, (char*)val, context->state->program_alc);
}

void Parser_parseAny(Token **token_stream, Type expect, ParserContext *context, ParserResult *out_result) {
	EParserState state = PARSER_STATE_NONE;
	Token *token = *token_stream;

	dispatch: {
		switch (token->type) {
			case TOKEN_TYPE_INPUT_END: goto handle_input_end;
			case TOKEN_TYPE_IDENTIFIER: goto handle_identifier;
			case TOKEN_TYPE_BRACE_OPEN: goto handle_brace;
			case TOKEN_TYPE_NUMERIC_LITERAL: goto handle_num;
			default:;
		}
	}

	if (0) handle_brace: switch (state) {
		case PARSER_STATE_NONE: {
			Parser_parseBracedExpression(token_stream, TYPE_ANY, context, out_result);
			return;
		}

		case PARSER_STATE_DSP_ID:
		case PARSER_STATE_DSP_VAL: 
			goto handle_call;

		default: goto invalid_state;
	}

	if (0) handle_call: switch (state) {
		case PARSER_STATE_DSP_ID:
		default: goto invalid_state;
	}

	if (0) handle_identifier: switch (state) {
		case PARSER_STATE_NONE: {
			Vector members;
			Vector_create(&members, sizeof(MemberValuePair));
			Vector_init(&members, 32, context->tmp_alc);
			ParserFrame_find(context->frame, token->str, &members, context->tmp_alc);

			MemberValuePair *it = Vector_begin(&members);
			MemberValuePair *end = Vector_end(&members);

			for (; it < end; it++) {
				
			}

			break;
		}

		default: goto invalid_state;
	}

	if (0) handle_num: switch (state) {
		case PARSER_STATE_NONE: {
			ParserResult result = ParserResult_NULL;
			Parser_parseNumber(token_stream, TYPE_ANY, context, &result);
			if (ParserResult_handle(&result, context)) goto early_exit;
			state = PARSER_STATE_DSP_VALUE;
		}
		default: goto invalid_state;
	}

	if (0) invalid_state: {
		Parser_message();
		return;
	}

	if (0) early_exit: {
		out_result->code = PARSER_
		return;
	}
	
	next_token: {
		(*token_stream)++;
		token = *token_stream;
		goto dispatch;
	}
	
	handle_input_end:;
	return;
}
