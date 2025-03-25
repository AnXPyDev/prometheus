bool ParseTree_sub_call(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Array funcs = *(Array*)payload;

	ParserResult result = ParserResult_NULL;
	Parser_parseCall(funcs, ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Token *token = TokenStream_next(ts);
	if (token->type != TOKEN_TYPE_BRACE_CLOSE) {
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Expected closing brace for arglist", this->result);
		return false;
	}

	return ParseTree_extendStateByNode(this, statep, result.node);
}

void ParseTree_branch_call(
	ParseTree *this, Token *token, ParseTreeState *state
) {
	ParseTreeState_FUNCTION *state_fun = (ParseTreeState_FUNCTION*)state;

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(Array));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_call;
	*(Array*)opt->payload = state_fun->funcs;

	ParseTree_pushOption(this, opt);
}

bool ParseTree_sub_unary(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Array funcs = *(Array*)payload;

	ParserResult result = ParserResult_NULL;
	Parser_parseCallUnary(funcs, ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	return ParseTree_extendStateByNode(this, statep, result.node);
}


void ParseTree_branch_operator_unary(
	ParseTree *this, Token *token, ParseTreeState *state, Array funcs
) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(Array));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_unary;
	*(Array*)opt->payload = funcs;

	ParseTree_pushOption(this, opt);
}

bool ParseTree_sub_binary(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ParserResult result = ParserResult_NULL;
	ParseTree_stateToNode(this, *statep, &result);

	if (Parser_checkfwd(&result, this->result)) return false;

	Array funcs = *(Array*)payload;

	ParserResult call_result = ParserResult_NULL;
	Parser_parseCallBinary(funcs, result.node, ts, this->ctx, &call_result);
	if (Parser_checkfwd(&result, this->result)) return false;

	ParseTreeState_NODE *state_node = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	state_node->header.type = PARSETREE_STATE_NODE;
	state_node->node = call_result.node;

	*statep = (ParseTreeState*)state_node;

	return true;
}

void ParseTree_branch_operator_binary(
	ParseTree *this, Token *token, ParseTreeState *state, Array funcs)
{
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(Array));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_binary;
	*(Array*)opt->payload = funcs;

	ParseTree_pushOption(this, opt);
}
