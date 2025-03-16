void Parser_setupKeyword(ParserFrame *frame, const char *id, int keyword_code, Allocator alc) {
	int *v = Allocator_malloc(alc, sizeof(int));
	*v = keyword_code;

	*ParserFrame_setupMemberWithValue(
		frame, strview(id), PrimitiveType_upcast(PRIMITIVE_TYPE_KEYWORD)
	) = v;
}

void Parser_setupBuiltins(ParserFrame *frame, Allocator alc) {
	// control
	Parser_setupKeyword(frame, "return", PARSER_KEYWORD_RETURN, alc);
	Parser_setupKeyword(frame, "throw", PARSER_KEYWORD_THROW, alc);
	Parser_setupKeyword(frame, "break", PARSER_KEYWORD_BREAK, alc);
	Parser_setupKeyword(frame, "continue", PARSER_KEYWORD_CONTINUE, alc);
	Parser_setupKeyword(frame, "emit", PARSER_KEYWORD_EMIT, alc);
	Parser_setupKeyword(frame, "jump", PARSER_KEYWORD_JUMP, alc);
	Parser_setupKeyword(frame, "exit", PARSER_KEYWORD_EXIT, alc);

	Parser_setupKeyword(frame, "=", PARSER_KEYWORD_SET, alc);
	Parser_setupKeyword(frame, "*=", PARSER_KEYWORD_SET_PTR, alc);

	Parser_setupKeyword(frame, "<@", PARSER_KEYWORD_GET_PTR, alc);
	Parser_setupKeyword(frame, ">@", PARSER_KEYWORD_TAKE_PTR, alc);

	Parser_setupKeyword(frame, "@", PARSER_KEYWORD_PTR, alc);

	Parser_setupKeyword(frame, "if", PARSER_KEYWORD_IF, alc);
	Parser_setupKeyword(frame, "else", PARSER_KEYWORD_ELSE, alc);
	Parser_setupKeyword(frame, "loop", PARSER_KEYWORD_LOOP, alc);

	Parser_setupKeyword(frame, "print", PARSER_KEYWORD_PRINT, alc);
}
