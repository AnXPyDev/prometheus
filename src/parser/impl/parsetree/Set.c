bool ParseTree_sub_setMember(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ParseTreeState_MEMBER *st_member = *(ParseTreeState_MEMBER**)payload;
	Member *member = st_member->member;
	
	switch (st_member->header.type) {
		default: return false;
		case PARSETREE_STATE_MEMBER: goto set_member;
		case PARSETREE_STATE_MEMBER_ELEMENT: goto set_member_element;
	}


	Node node;

	if (0) set_member: {
		ParserResult result = ParserResult_NULL;
		result.expect = st_member->member->type;

		Parser_parseNode(ts, this->ctx, &result);
		if (Parser_checkfwd(&result, this->result)) return false;
		node = SetNode_create(member, result.node, this->ctx->program_alc);
	}

	if (0) set_member_element: {
		ParseTreeState_MEMBER_ELEMENT *st_elm = (ParseTreeState_MEMBER_ELEMENT*)st_member;

		ParserResult result = ParserResult_NULL;
		result.expect = st_elm->type;

		Parser_parseNode(ts, this->ctx, &result);
		if (Parser_checkfwd(&result, this->result)) return false;

		node = SetElementNode_create(
			member, st_elm->offset, st_elm->type, result.node, this->ctx->program_alc
		);
	}

	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = node;

	*statep = (ParseTreeState*)node_state;

	this->done = true;

	return true;
}

void ParseTree_branch_setMember(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTreeState_MEMBER*));

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_setMember;
	*(ParseTreeState_MEMBER**)opt->payload = (ParseTreeState_MEMBER*)state;

	ParseTree_pushOption(this, opt);
}
