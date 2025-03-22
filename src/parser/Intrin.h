typedef enum {
	PARSER_INTRIN_DECLARATION
} EParserIntrinType;

typedef struct {
	EParserIntrinType type;
} ParserIntrin;

typedef struct {
	ParserIntrin header;
	Type type;
	Qualifier qualifier;
	BufferView identifier;
} ParserIntrin_DECLARATION;
