typedef enum {
	PARSER_HINT_NEW_EXPRESSION,
	PARSER_HINT_VARIABLE_DECLARATION,
	PARSER_HINT_IMPLICIT_END,
	PARSER_HINT_BRACE_OPEN,
	PARSER_HINT_BRACE_CLOSE,
	PARSER_HINT_VALUE_TYPE,
	PARSER_HINT_VALUE_FUNCTION,
	PARSER_HINT_IDENTIFIER,
} EParserHint;

typedef enum {
	PARSER_DISPATCH_FUNCTION_CALL,
	PARSER_DISPATCH_DECLARATION,
} EParserDispatch;

void Parser_setupHintDispatchMap(HashMap *map) {
	#define MAP(hint_array, dispatch) *(EParserDispatch*)HashMap_add(map, toBufferView(hint_array)) = dispatch
	#define MAPPING(dispatch, ...) { const EParserHint hints[] = { __VA_ARGS__ }; MAP(hints, dispatch); }

	MAPPING(PARSER_DISPATCH_FUNCTION_CALL,
		PARSER_HINT_NEW_EXPRESSION,
		PARSER_HINT_VALUE_FUNCTION,
		PARSER_HINT_BRACE_OPEN
	);

	#undef MAPPING
	#undef MAP
}

#define alc (this->allocator)

Expression Parser_parse(Parser *this, ArrayView tokens) {
	const Token *it = (Token*)tokens.data;
	const Token *it_end = it + tokens.size;

	HashMap dispatchMap = HashMap_new(alc, sizeof(EParserDispatch));
	Parser_setupHintDispatchMap(&dispatchMap);

	Vector hintStack = Vector_new(alc, sizeof(EParserHint));
	Vector_init(&hintStack, 16);

	Vector frameStack = Vector_new(alc, sizeof(Expression_Frame*));
	Vector_init(&frameStack, 16);

	Vector expressions = Vector_new(alc, sizeof(Expression));
	Vector_init(&expressions, 16);

	Expression_Frame *frame;

	#define PUSH_FRAME() { \
		frame = Allocator_new(this->allocator, Expression_Frame); \
		*(Expression_Frame**)Vector_push(&frameStack) = frame; \
	}

	#define POP_FRAME() { frame = *(Expression_Frame**)Vector_pop(&frameStack); }

	#define PUSH_HINT(hint) { *(EParserHint*)Vector_push(&hintStack) = (hint); }
	#define CLEAR_HINTS() { Vector_clear(&hintStack); }

	PUSH_FRAME();

	new_expression: {
		CLEAR_HINTS();
		PUSH_HINT(PARSER_HINT_NEW_EXPRESSION);
		goto next_token;
	}

	next_token: {
		it++;
		if (it >= it_end) {
			goto quit;
		}
	}

	dispatch: {

	}


	quit: {
	}

	return Expression_NULL;

	#undef PUSH_FRAME
	#undef POP_FRAME
	#undef PUSH_HINT
	#undef CLEAR_HINTS
}

#undef alc
