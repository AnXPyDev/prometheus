typedef struct {
	ParseTreeState header;
	Type type;
} ParseTreeState_TYPE;

void ParseTree_branch_type(
	ParseTree *this, Token *token, ParseTreeState *state, Type T
) {

	ParseTreeOption_State *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTreeState_TYPE));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_STATE;

	ParseTreeState_TYPE *state_type = (ParseTreeState_TYPE*)opt->state;
	state_type->header.type = PARSETREE_STATE_TYPE;
	state_type->type = T;

	ParseTree_dispatch(this, token + 1, (ParseTreeState*)state_type);

	ParseTree_pushOption(this, opt);
}
