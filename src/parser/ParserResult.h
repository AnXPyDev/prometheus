typedef enum {
	PARSER_RESULT_OK = 0,

	PARSER_RESULT_WARNING,

	PARSER_RESULT_ERROR,

	PARSER_RESULT_PANIC,
	PARSER_RESULT_PANIC_INVALID_STATE,
	PARSER_RESULT__END
} EParserResultCode;

const char *EParserResultCode_repr[PARSER_RESULT__END] = {
	[PARSER_RESULT_OK] = "PARSER_RESULT_OK",
	[PARSER_RESULT_WARNING] = "PARSER_RESULT_WARNING",
	[PARSER_RESULT_ERROR] = "PARSER_RESULT_ERROR",
	[PARSER_RESULT_PANIC] = "PARSER_RESULT_PANIC",
};

typedef struct {
	EParserResultCode code;
	int flags;
	Type expect;
	Node node;
} ParserResult;

const ParserResult ParserResult_NULL = {
	.code = PARSER_RESULT_OK,
	.node = Node_NULL,
	.expect = Type_NULL,
	.flags = 0
};
