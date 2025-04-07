bool ParseTree_sub_condition(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ParserResult result = ParserResult_NULL;
	Parser_parseCondition(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = result.node;

	*statep = (ParseTreeState*)node_state;
	this->done = true;
	return true;
}

void ParseTree_branch_condition(
	ParseTree *this, Token *token, ParseTreeState *state
) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_condition;

	ParseTree_pushOption(this, opt);
}
