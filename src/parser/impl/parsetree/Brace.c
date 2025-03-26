bool ParseTree_sub_braced(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ParserResult result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_EXPLICIT_END;

	if (this->flags & PARSENODE_FLAG_NO_MARCH) {
		result.expect = this->result->expect;
	}

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Token *token = TokenStream_next(ts);
	if (token->type != TOKEN_TYPE_BRACE_CLOSE) {
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Expected closing brace", this->result);
		return false;
	}

	return ParseTree_extendStateByNode(this, statep, result.node);
}

void ParseTree_branch_brace(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		case PARSETREE_STATE_FUNCTION: goto handle_call;
		case PARSETREE_STATE_TYPE: goto handle_fanon;
		case PARSETREE_STATE_DECLARATION: goto handle_fdecl;
		//case PARSETREE_STATE_NONE: goto handle_braced;
		default: goto handle_braced;
	}
		

	if (0) handle_braced: {
		ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
		opt->header.next_token = token + 1;
		opt->header.type = PARSETREE_OPTION_SUB;
		opt->subf = &ParseTree_sub_braced;
		ParseTree_pushOption(this, opt);
	}

	if (0) handle_call: ParseTree_branch_call(this, token, state);

	if (0) handle_fanon: ParseTree_branch_fanon(this, token, state);
	if (0) handle_fdecl: ParseTree_branch_fdecl(this, token, state);
}

bool ParseTree_sub_frame(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ParserResult result = ParserResult_NULL;
	Parser_parseFrame(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Token *token = TokenStream_next(ts);
	if (token->type != TOKEN_TYPE_CBRACE_CLOSE) {
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Expected closing brace", this->result);
		return false;
	}

	ParseTreeState_NODE *state_node = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	state_node->header.type = PARSETREE_STATE_NODE;
	state_node->node = result.node;

	*statep = (ParseTreeState*)state_node;
	return true;
}

void ParseTree_branch_cbrace(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		default: return;
		case PARSETREE_STATE_NONE:;
	}

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_frame;

	ParseTree_pushOption(this, opt);
}
