typedef struct {
	ParseTreeState state;
	Qualifier qualifier;
} ParseTreeState_QUALIFIER;

typedef struct {
	ParseTreeState state;
	Type type;
	Qualifier qualifier;
} ParseTreeState_TYPE_AND_QUALIFIER;

void ParseTree_branch_qualifier(
	ParseTree *this, Token *token, ParseTreeState *state, Qualifier qualifier
) {

}
