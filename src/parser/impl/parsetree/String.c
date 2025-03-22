bool ParseTree_sub_string(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	Token *token = *(Token**)payload;

	ParserResult result = ParserResult_NULL;
	Parser_parseString(token, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	ParseTreeState_NODE *newstate = ParseTree_stalloc(
		this, sizeof(ParseTreeState_NODE)
	);

	newstate->header.type = PARSETREE_STATE_NODE;
	newstate->node = result.node;

	*statep = (ParseTreeState*)newstate;

	return true;
}

void ParseTree_branch_string(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		default:;
			return;
		case PARSETREE_STATE_NONE:;
	}

	{
		// parse number subroutine

		ParseTreeOption_Sub *option = ParseTree_stalloc(
			this, sizeof(ParseTreeOption_Sub) + sizeof(Token*)
		);

		option->header = (ParseTreeOption) {
			.type = PARSETREE_OPTION_SUB,
			.next_token = token + 1
		};

		option->subf= &ParseTree_sub_string;
		*(Token**)option->payload = token;

		ParseTree_pushOption(this, option);
	}
}
