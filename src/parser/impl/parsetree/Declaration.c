typedef struct {
	Qualifier qualifier;
	Type type;
	BufferView identifier;
} ParseTree_DECLARATION;

typedef struct {
	ParseTreeState header;
	ParseTree_DECLARATION info;
} ParseTreeState_DECLARATION;

bool ParseTree_sub_declaration(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	Token *here = TokenStream_probe(ts);
	if (this->flags & PARSENODE_FLAG_NO_DECLARE) {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Declaration not allowed here", this->result);
		return false;
	}

	ParseTree_DECLARATION *info = payload;

	MemberValuePair mvp = MemberValuePair_NULL;
	ParserFrame_findOneLocal(this->ctx->frame, info->identifier, &mvp);

	if (mvp.member) {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Redeclaration of identifier within frame", this->result);
		return false;
	}

	Member *member = MemberList_add(this->ctx->frame->memberlist, info->identifier, info->qualifier, info->type);

	ParseTreeState_MEMBER *state_member = ParseTree_stalloc(this, sizeof(ParseTreeState_MEMBER));
	state_member->header.type = PARSETREE_STATE_MEMBER;
	state_member->member = member;

	*statep = (ParseTreeState*)state_member;
	return true;
}

void ParseTree_branch_declaration(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		default: return;
		case PARSETREE_STATE_TYPE: goto handle_type;
		case PARSETREE_STATE_TYPE_AND_QUALIFIER: goto handle_qual;
	}

	ParseTree_DECLARATION info;

	if (0) handle_type: {
		ParseTreeState_TYPE *state_type = (ParseTreeState_TYPE*)state;
		info.qualifier = Qualifier_NULL;
		info.type = state_type->type;
	}

	if (0) handle_qual: {
		ParseTreeState_TYPE_AND_QUALIFIER *state_tq = (ParseTreeState_TYPE_AND_QUALIFIER*)state;
		info.qualifier = state_tq->qualifier;
		info.type = state_tq->type;
	}

	info.identifier = token->str;

	ParseTreeState_DECLARATION *state_decl = NULL;

	if (this->flags & PARSENODE_FLAG_DEFER_DECLARATION) {
		ParseTreeOption_State *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTreeState_DECLARATION));
		opt->header.next_token = token + 1;
		opt->header.type = PARSETREE_OPTION_STATE;

		state_decl = (ParseTreeState_DECLARATION*)opt->state;
		state_decl->header.type = PARSETREE_STATE_DECLARATION;
		state_decl->info = info;

		ParseTree_pushOption(this, opt);
	} else {
		ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTree_DECLARATION));
		opt->header.next_token = token + 1;
		opt->header.type = PARSETREE_OPTION_SUB;
		opt->subf = &ParseTree_sub_declaration;

		*(ParseTree_DECLARATION*)opt->payload = info;

		ParseTree_pushOption(this, opt);
	}

	if (!state_decl) {
		state_decl = ParseTree_stalloc(this, sizeof(ParseTreeState_DECLARATION));
		state_decl->header.type = PARSETREE_STATE_DECLARATION;
		state_decl->info = info;
	}

	ParseTree_dispatch(this, token + 1, (ParseTreeState*)state_decl);

}
