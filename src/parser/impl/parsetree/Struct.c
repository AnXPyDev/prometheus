bool ParseTree_sub_struct(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	Token *token = TokenStream_next(ts);
	if (token->type != TOKEN_TYPE_CBRACE_OPEN) {
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Expected opening cbrace here", this->result);
		return false;
	}

	ParserResult result = ParserResult_NULL;
	MemberList *ml = Parser_parseStructMembers(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	token = TokenStream_next(ts);
	if (token->type != TOKEN_TYPE_CBRACE_CLOSE) {
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Expected closing cbrace here", this->result);
		return false;
	}

	if (ml->members.size == 0) {
		MemberList_destroy(ml);
		Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "Empty struct not allowed", this->result);
		return false;
	}

	MemberListInfo *info = StructType_generateMemberListInfo(ml, this->ctx->program_alc);

	Type ST = StructType_create(ml, info, this->ctx->tmp_alc);

	if ((*statep)->type == PARSETREE_STATE_QUALIFIER) {
		ParseTreeState_QUALIFIER *state_qual = (ParseTreeState_QUALIFIER*)*statep;
		ST = QualifierType_create(state_qual->qualifier, ST, this->ctx->tmp_alc);
	}

	ParseTreeState_TYPE *nstate = ParseTree_stalloc(this, sizeof(ParseTreeState_TYPE));
	nstate->header.type = PARSETREE_STATE_TYPE;
	nstate->type = ST;

	*statep = (ParseTreeState*)nstate;

	return true;
}

void ParseTree_branch_struct(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		default: return;
		case PARSETREE_STATE_NONE:;
		case PARSETREE_STATE_QUALIFIER:;
	}

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_struct;
	ParseTree_pushOption(this, opt);
}
