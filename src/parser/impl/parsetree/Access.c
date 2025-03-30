void ParseTree_branch_accessByIdentifier(
	ParseTree *this, Token *token, ParseTreeState *state
) {
	BufferView id = token->str;

	switch (state->type) {
		default: return;
		case PARSETREE_STATE_ACCESS_MEMBER:
			goto handle_member;
		case PARSETREE_STATE_ACCESS_NODE:
			goto handle_node;
	}

	if (0) handle_member: {
		Member *member = ((ParseTreeState_MEMBER*)state)->member;
		Type MT = Type_strip(member->type);
		
		if (Type_isPointerType(MT)) {
			PointerType *ptr = MT.object;
			Type PT = Type_strip(ptr->T);

			if (!Type_isStructType(PT)) return;

			StructType *strct = PT.object;

			Member *st_member = StructType_getMember(strct, id);
			if (!st_member) return;

			ParseTreeOption_State *opt = ParseTree_stalloc(
				this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_NODE)
			);

			opt->header.next_token = token + 1;
			opt->header.type = PARSETREE_OPTION_STATE;

			ParseTreeState_NODE *nstate = (ParseTreeState_NODE*)opt->state;

			nstate->header.type = PARSETREE_STATE_NODE;
			nstate->node = GetPointerElementNode_create(
				GetNode_create(member, this->ctx->program_alc),
				StructType_getMemberOffset(strct, st_member),
				st_member->type, this->ctx->program_alc
			);

			ParseTree_pushOption(this, opt);

		} else if (Type_isStructType(MT)) {
			StructType *strct = MT.object;
			Member *st_member = StructType_getMember(strct, id);
			if (!st_member) return;

			ParseTreeOption_State *opt = ParseTree_stalloc(
				this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_MEMBER_ELEMENT)
			);

			opt->header.next_token = token + 1;
			opt->header.type = PARSETREE_OPTION_STATE;

			ParseTreeState_MEMBER_ELEMENT *nstate = (ParseTreeState_MEMBER_ELEMENT*)opt->state;

			nstate->member.header.type = PARSETREE_STATE_MEMBER_ELEMENT;
			nstate->member.member = member;
			nstate->type = st_member->type;
			nstate->offset = StructType_getMemberOffset(strct, st_member);

			ParseTree_pushOption(this, opt);
		}
	}

	if (0) handle_node: {
		Node node = ((ParseTreeState_NODE*)state)->node;
		Type RT = Type_strip(Node_resultType(node, this->ctx->tmp_alc));

		if (Type_isPointerType(RT)) {
			PointerType *ptr = RT.object;
			Type PT = Type_strip(ptr->T);

			if (!Type_isStructType(PT)) return;

			StructType *strct = PT.object;

			Member *st_member = StructType_getMember(strct, id);
			if (!st_member) return;

			ParseTreeOption_State *opt = ParseTree_stalloc(
				this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_NODE)
			);

			opt->header.next_token = token + 1;
			opt->header.type = PARSETREE_OPTION_STATE;

			ParseTreeState_NODE *nstate = (ParseTreeState_NODE*)opt->state;

			nstate->header.type = PARSETREE_STATE_NODE;
			nstate->node = GetPointerElementNode_create(
				node, StructType_getMemberOffset(strct, st_member),
				st_member->type, this->ctx->program_alc
			);

			ParseTree_pushOption(this, opt);

		} else if (Type_isStructType(RT)) {
			StructType *strct = RT.object;
			Member *st_member = StructType_getMember(strct, id);
			if (!st_member) return;

			ParseTreeOption_State *opt = ParseTree_stalloc(
				this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_NODE_ELEMENT)
			);

			opt->header.next_token = token + 1;
			opt->header.type = PARSETREE_OPTION_STATE;

			ParseTreeState_NODE *nstate = (ParseTreeState_NODE*)opt->state;

			nstate->header.type = PARSETREE_STATE_NODE;
			nstate->node = GetValueElementNode_create(
				node, StructType_getMemberOffset(strct, st_member), st_member->type,
				this->ctx->program_alc
			);

			ParseTree_pushOption(this, opt);
		}
	}
}

void ParseTree_branch_access(
	ParseTree *this, Token *token, ParseTreeState *state
) {
	switch (state->type) {
		default: return;
		case PARSETREE_STATE_MEMBER:
			goto handle_member;
		case PARSETREE_STATE_NODE:
			goto handle_node;
	}

	ParseTreeState *next_state;

	if (0) handle_member: {
		ParseTreeState_MEMBER *state_member = (ParseTreeState_MEMBER*)state;
		ParseTreeState_MEMBER *nstate = ParseTree_stalloc(this, sizeof(ParseTreeState_MEMBER));
		nstate->header.type = PARSETREE_STATE_ACCESS_MEMBER;
		nstate->member = state_member->member;
		next_state = (ParseTreeState*)nstate;
	}
	
	if (0) handle_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		ParseTreeState_NODE *nstate = ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
		nstate->header.type = PARSETREE_STATE_ACCESS_NODE;
		nstate->node = state_node->node;
		next_state = (ParseTreeState*)nstate;
	}

	ParseTree_dispatch(this, token + 1, next_state);
}
