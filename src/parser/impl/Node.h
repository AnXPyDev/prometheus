#define FLAG(s) PARSENODE_FLAG_##s

enum {
	FLAG(NONE) = 0,
	FLAG(NO_EXPLICIT_END) = 1 << 0,
	FLAG(NO_CONSUME_EXPLICIT_END) = 1 << 1,
};

#undef FLAG

void Parser_parseNode(int flags, TokenStream*, ParserContext*, ParserResult*);
