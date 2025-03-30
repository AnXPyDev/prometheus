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
	
	if (Type_equalPrimitive(Type_strip(info->type), PRIMITIVE_TYPE_AUTO)) {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Cannot declare member with type auto, an assignment must follow this declaration", this->result);
		return false;
	}

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

bool ParseTree_sub_setDeclaration(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
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

	bool isAuto = Type_equalPrimitive(Type_strip(info->type), PRIMITIVE_TYPE_AUTO);

	ParserResult result = ParserResult_NULL;
	if (!isAuto) {
		result.expect = info->type;
	}

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Type MT = info->type;
	if (isAuto) {
		MT = Node_resultType(result.node, this->ctx->tmp_alc);
		if (Type_isQualifierType(info->type)) {
			QualifierType *qt = info->type.object;
			MT = QualifierType_create(qt->Q, MT, this->ctx->tmp_alc);
		}
	}
	
	Member *member = MemberList_add(this->ctx->frame->memberlist, info->identifier, info->qualifier, MT);

	Node node = SetNode_create(member, result.node, this->ctx->program_alc);

	if (Node_isValueNode(result.node)) {
		ValueNode *val = result.node.object;

		void *data = ParserFrame_ensureValue(this->ctx->frame, member);

		if (Type_equalPrimitive(Type_strip(val->T), PRIMITIVE_TYPE_TYPE)) {
			Type VT = *(Type*)val->data;
			*(Type*)data = Type_copy(VT, this->ctx->program_alc);
		}
	}

	ParseTreeState_NODE *node_state = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
	node_state->header.type = PARSETREE_STATE_NODE;
	node_state->node = node;

	*statep = (ParseTreeState*)node_state;

	this->done = true;

	return true;
}

void ParseTree_branch_setDeclaration(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeState_DECLARATION *state_decl = (ParseTreeState_DECLARATION*)state;

	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTree_DECLARATION));

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_setDeclaration;
	*(ParseTree_DECLARATION*)opt->payload = state_decl->info;

	ParseTree_pushOption(this, opt);

}
