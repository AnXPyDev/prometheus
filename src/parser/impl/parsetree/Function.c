void ParseTree_branch_function(
	ParseTree *this, Token *token, ParseTreeState *state, Array funcs
) {
	ParseTreeState_FUNCTION *state_fun = ParseTree_stalloc(this, sizeof(ParseTreeState_FUNCTION));
	state_fun->header.type = PARSETREE_STATE_FUNCTION;
	state_fun->funcs = funcs;

	ParseTree_dispatch(this, token + 1, (ParseTreeState*)state_fun);
}

void ParseTree_branch_operator_unary(
	ParseTree *this, Token *token, ParseTreeState *state, Array funcs
) {

}

void ParseTree_branch_operator_binary(
	ParseTree *this, Token *token, ParseTreeState *state, Array funcs)
{

}

bool ParseTree_sub_fanon(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Type RT = *(Type*)payload;
	
	ParserResult result = ParserResult_NULL;
	Parser_parseFunction_anonymous(ts, this->ctx, &result, RT);
	if (Parser_checkfwd(&result, this->result)) return false;
	
	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = result.node;

	*statep = (ParseTreeState*)node_state;
	return true;
}

void ParseTree_branch_fanon(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeState_TYPE *state_type = (ParseTreeState_TYPE*)state;

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(Type));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_fanon;
	*(Type*)opt->payload = state_type->type;

	ParseTree_pushOption(this, opt);
}

bool ParseTree_sub_fdecl(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Token *here = TokenStream_probe(ts);
	if (this->flags & PARSENODE_FLAG_NO_DECLARE) {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Function declaration not allowed here", this->result);
		return false;
	}

	ParseTree_DECLARATION *info = payload;

	/*
	MemberValuePair mvp = MemberValuePair_NULL;
	ParserFrame_findOneLocal(this->ctx->frame, info->identifier, &mvp);

	if (mvp.member) {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Redeclaration of identifier within frame", this->result);
		return false;
	}
	*/

	ParserIntrin_DECLARATION intrdecl = {
		.header.type = PARSER_INTRIN_DECLARATION,
		.type = info->type,
		.identifier = info->identifier,
		.qualifier = info->qualifier
	};

	ParserResult result = ParserResult_NULL;
	Parser_parseFunction_declaration(ts, this->ctx, &result, &intrdecl);
	if (Parser_checkfwd(&result, this->result)) return false;
	
	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = result.node;

	*statep = (ParseTreeState*)node_state;
	return true;
}

void ParseTree_branch_fdecl(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeState_DECLARATION *state_decl = (ParseTreeState_DECLARATION*)state;

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTree_DECLARATION));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_fdecl;
	*(ParseTree_DECLARATION*)opt->payload = state_decl->info;

	ParseTree_pushOption(this, opt);
}
