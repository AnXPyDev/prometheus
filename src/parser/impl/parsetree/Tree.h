typedef struct {
	ParserContext *ctx;
	Vector_Alc options;
	Allocator state_alc;
	ParserResult *result;
	int flags;
	bool done;
} ParseTree;

typedef enum {
	PARSETREE_STATE_NONE = 0,
	PARSETREE_STATE_NODE,
	PARSETREE_STATE_ACCESS_NODE,
	PARSETREE_STATE_EXTEND_NODE,
	PARSETREE_STATE_TYPE,
	PARSETREE_STATE_IDENTIFIER,
	PARSETREE_STATE_MEMBER,
	PARSETREE_STATE_ACCESS_MEMBER,
	PARSETREE_STATE_FUNCTION,
	PARSETREE_STATE_FUNCTION_CANDIDATES,
	PARSETREE_STATE_DECLARATION,
	PARSETREE_STATE_QUALIFIER,
	PARSETREE_STATE_TYPE_AND_QUALIFIER,
	PARSETREE_STATE_MEMBER_ELEMENT,
	PARSETREE_STATE_NODE_ELEMENT,
	PARSETREE_STATE__END
} EParseTreeStateType;

const char *EParseTreeStateType_REPR[PARSETREE_STATE__END] = {
	[PARSETREE_STATE_NONE] = "PARSETREE_STATE_NONE",
	[PARSETREE_STATE_NODE] = "PARSETREE_STATE_NODE",
	[PARSETREE_STATE_NODE_ELEMENT] = "PARSETREE_STATE_NODE_ELEMENT",
	[PARSETREE_STATE_ACCESS_NODE] = "PARSETREE_STATE_ACCESS_NODE",
	[PARSETREE_STATE_EXTEND_NODE] = "PARSETREE_STATE_EXTEND_NODE",
	[PARSETREE_STATE_TYPE] = "PARSETREE_STATE_TYPE",
	[PARSETREE_STATE_IDENTIFIER] = "PARSETREE_STATE_IDENTIFIER",
	[PARSETREE_STATE_MEMBER] = "PARSETREE_STATE_MEMBER",
	[PARSETREE_STATE_MEMBER_ELEMENT] = "PARSETREE_STATE_MEMBER_ELEMENT",
	[PARSETREE_STATE_ACCESS_MEMBER] = "PARSETREE_STATE_ACCESS_MEMBER",
	[PARSETREE_STATE_FUNCTION] = "PARSETREE_STATE_FUNCTION",
	[PARSETREE_STATE_FUNCTION_CANDIDATES] = "PARSETREE_STATE_FUNCTION_CANDIDATES",
	[PARSETREE_STATE_DECLARATION] = "PARSETREE_STATE_DECLARATION",
	[PARSETREE_STATE_QUALIFIER] = "PARSETREE_STATE_QUALIFIER",
	[PARSETREE_STATE_TYPE_AND_QUALIFIER] = "PARSETREE_STATE_TYPE_AND_QUALIFIER",
};

typedef struct {
	EParseTreeStateType type;
} ParseTreeState;

typedef struct {
	ParseTreeState header;
	Node node;
} ParseTreeState_NODE;

typedef struct {
	ParseTreeState_NODE state_node;
	ParseTreeState *previous;
} ParseTreeState_EXTEND_NODE;

typedef enum {
	PARSETREE_SUB_OK = 0,
	PARSETREE_SUB_OK_RETURN,
	PARSETREE_SUB_ERROR
} EParseTree_SubResult;

typedef bool (*ParseTree_sub_fn_t)(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
);

typedef enum {
	PARSETREE_OPTION_STATE,
	PARSETREE_OPTION_SUB,
} EParseTreeOption;

typedef struct {
	Token *next_token;
	EParseTreeOption type;
} ParseTreeOption;

typedef struct {
	ParseTreeOption header;
	ParseTree_sub_fn_t subf;
	char payload[];
} ParseTreeOption_Sub;

typedef struct {
	ParseTreeOption header;
	char state[];
} ParseTreeOption_State;

void ParseTree_pushOption(ParseTree *this, void *option) {
	*(ParseTreeOption**)Vector_push(this->options.vec, this->options.alc) = option;
}

void *ParseTree_stalloc(ParseTree *this, Size size) {
	return Allocator_calloc(this->state_alc, size);
}

void ParseTree_dispatch(ParseTree *this, Token *token, ParseTreeState *state);

bool ParseTree_extendStateByNode(ParseTree *this, ParseTreeState **statep, Node node);

void ParseTree_stateToNode(ParseTree *this, ParseTreeState *state, ParserResult *out);
