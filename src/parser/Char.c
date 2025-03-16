typedef int ParserChar;

char ParserChar_toChar(ParserChar c) {
	return (char)c;
}

typedef enum {
	PARSER_CHAR_NUMERIC = 1 << 0,
	PARSER_CHAR_ALPHA = 1 << 1,
	PARSER_CHAR_IDENTIFIER = 1 << 2,
	PARSER_CHAR_IDENTIFIER_BEGIN = 1 << 3,
	PARSER_CHAR_RESTRICT = 1 << 4,
	PARSER_CHAR_NEWLINE = 1 << 5,
	PARSER_CHAR_NUMERIC_DELIMITER = 1 << 6,
	PARSER_CHAR_NUMERIC_DECIMAL_POINT = 1 << 7,
	PARSER_CHAR_WHITESPACE = 1 << 8,
	PARSER_CHAR_STRING_DELIMITER = 1 << 9,
	PARSER_CHAR_STRING_ESCAPE = 1 << 10
} EParserChar_Flags;

int g_Parser_CharFlags[256] = {0};
ETokenType g_Parser_RestrictTypes[256] = {0};

void g_Parser_setupCharLookupTable(void) {
	#define tbl g_Parser_CharFlags
	#define rtbl g_Parser_RestrictTypes

	rtbl['('] = TOKEN_TYPE_BRACE_OPEN;
	rtbl[')'] = TOKEN_TYPE_BRACE_CLOSE;
	rtbl['{'] = TOKEN_TYPE_CBRACE_OPEN;
	rtbl['}'] = TOKEN_TYPE_CBRACE_CLOSE;
	rtbl['['] = TOKEN_TYPE_SBRACE_OPEN;
	rtbl[']'] = TOKEN_TYPE_SBRACE_CLOSE;
	rtbl[';'] = TOKEN_TYPE_END;
	rtbl[','] = TOKEN_TYPE_LIST_DELIMITER;
	rtbl['.'] = TOKEN_TYPE_ACCESSOR;
	rtbl[':'] = TOKEN_TYPE_ACCESSOR;
	rtbl['/'] = TOKEN_TYPE_ACCESSOR;

	// newline
	tbl[10] = PARSER_CHAR_NEWLINE | PARSER_CHAR_WHITESPACE;
	tbl[13] = PARSER_CHAR_NEWLINE | PARSER_CHAR_WHITESPACE;
	tbl[' '] = PARSER_CHAR_WHITESPACE;
	tbl['\t'] = PARSER_CHAR_WHITESPACE;
	tbl['"'] = PARSER_CHAR_STRING_DELIMITER;
	tbl['\\'] = PARSER_CHAR_STRING_ESCAPE;

	const char chars_restrict[] = {
		'.', ',', ';', ':', '/', '(', ')', '{', '}', '[', ']'
	};
	const char chars_identifier_special[] = {
		'~', '!', '#', '$', '%', '^', '&', '*', '-', '_', '=', '+', '|', '\\', '?', '@', '<', '>'
	};

	for (uint32_t i = 0; i < sizeof(chars_restrict); i++) {
		tbl[(int)chars_restrict[i]] = PARSER_CHAR_RESTRICT;
	}
	
	for (uint32_t i = 0; i < sizeof(chars_identifier_special); i++) {
		tbl[(int)chars_identifier_special[i]] = PARSER_CHAR_IDENTIFIER | PARSER_CHAR_IDENTIFIER_BEGIN;
	}

	tbl['_'] |= PARSER_CHAR_NUMERIC_DELIMITER;
	tbl['.'] |= PARSER_CHAR_NUMERIC_DECIMAL_POINT;

	// letters
	for (int i = 'A'; i <= 'Z'; i++) {
		// uppercase
		tbl[i] = PARSER_CHAR_ALPHA | PARSER_CHAR_IDENTIFIER | PARSER_CHAR_IDENTIFIER_BEGIN;
		// lowercase
		tbl[i + 32] = PARSER_CHAR_ALPHA | PARSER_CHAR_IDENTIFIER | PARSER_CHAR_IDENTIFIER_BEGIN;
	}

	// numbers
	for (int i = '0'; i <= '9'; i++) {
		tbl[i] = PARSER_CHAR_NUMERIC | PARSER_CHAR_IDENTIFIER;
	}
	
	#undef tbl
	#undef rtbl
}
