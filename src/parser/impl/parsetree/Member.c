typedef struct {
	ParseTreeState header;
	Member *member;
} ParseTreeState_MEMBER;

void ParseTree_branch_member(
	ParseTree *this, Token *token, ParseTreeState *state, Member *member
) {
	switch (state->type) {
		default: return;
		case PARSETREE_STATE_NONE:;
	}

	ParseTreeOption_State *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_MEMBER));

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_STATE;

	ParseTreeState_MEMBER *member_state = (ParseTreeState_MEMBER*)opt->state;
	member_state->header.type = PARSETREE_STATE_MEMBER;
	member_state->member = member;

	ParseTree_pushOption(this, opt);

	ParseTree_dispatch(this, token + 1, (ParseTreeState*)member_state);
}

void ParseTree_dispatch_member(
	ParseTree *this, Token *token, ParseTreeState *state, MemberValuePair mvp
) {
	if (!mvp.value) goto skipval;

	Type T = mvp.member->type;

	if (Type_equalPrimitive(T, PRIMITIVE_TYPE_TYPE)) {
		Type typeval = *(Type*)mvp.value;
		ParseTree_branch_type(this, token, state, typeval);
	} else if (Type_equalPrimitive(T, PRIMITIVE_TYPE_KEYWORD)) {
		EParserKeyword kw = *(EParserKeyword*)mvp.value;
		ParseTree_branch_keyword(this, token, state, kw);
	} else if (Type_equalPrimitive(T, PRIMITIVE_TYPE_QUALIFIER)) {
		Qualifier qual = *(Qualifier*)mvp.value;
		ParseTree_branch_qualifier(this, token, state, qual);
	}

	skipval:;

	ParseTree_branch_member(this, token, state, mvp.member);
}
