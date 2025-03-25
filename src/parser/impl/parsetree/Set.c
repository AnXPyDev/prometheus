bool ParseTree_sub_setMember(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Member *member = *(Member**)payload;

	ParserResult result = ParserResult_NULL;
	result.expect = member->type;

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Node node = SetNode_create(member, result.node, this->ctx->program_alc);

	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = node;

	*statep = (ParseTreeState*)node_state;

	this->done = true;

	return true;
}

void ParseTree_branch_setMember(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeState_MEMBER *state_member = (ParseTreeState_MEMBER*)state;

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(Member*));

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_setMember;
	*(Member**)opt->payload = state_member->member;

	ParseTree_pushOption(this, opt);
}

void ParseTree_branch_setDeclaration(ParseTree *this, Token *token, ParseTreeState *state) {
}
