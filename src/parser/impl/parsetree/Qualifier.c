void ParseTree_branch_qualifier(
	ParseTree *this, Token *token, ParseTreeState *state, Qualifier qualifier
) {
	ParseTreeState *next_state;

	switch (state->type) {
		default: return;
		case PARSETREE_STATE_TYPE:
		case PARSETREE_STATE_TYPE_AND_QUALIFIER:
			goto with_type;
		case PARSETREE_STATE_NONE:
		case PARSETREE_STATE_QUALIFIER:;
			goto from_zero;
	}

	if (0) from_zero: {
		ParseTreeOption_State *opt = ParseTree_stalloc(
			this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_QUALIFIER)
		);

		opt->header.next_token = token +1;
		opt->header.type = PARSETREE_OPTION_STATE;

		ParseTreeState_QUALIFIER *state_qual = (ParseTreeState_QUALIFIER*)opt->state;
		state_qual->header.type = PARSETREE_STATE_QUALIFIER;

		next_state = (ParseTreeState*)state_qual;

		if (state->type == PARSETREE_STATE_NONE) {
			state_qual->qualifier = qualifier;
		} else if (state->type == PARSETREE_STATE_QUALIFIER) {
			ParseTreeState_QUALIFIER *qstate = (ParseTreeState_QUALIFIER*)state;

			Qualifier elements[] = { qstate->qualifier, qualifier };

			state_qual->qualifier = UnionQualifier_create((Array) {
				.data = elements, .size = 2
			}, this->state_alc);
		}

		//ParseTree_pushOption(this, opt);
		ParseTree_dispatch(this, token + 1, (ParseTreeState*)state_qual);
	}

	if (0) with_type: {
		ParseTreeOption_State *opt = ParseTree_stalloc(
			this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_TYPE_AND_QUALIFIER)
		);

		opt->header.next_token = token +1;
		opt->header.type = PARSETREE_OPTION_STATE;
		ParseTreeState_TYPE_AND_QUALIFIER *state_tq = 
			(ParseTreeState_TYPE_AND_QUALIFIER*)opt->state;

		next_state = (ParseTreeState*)state_tq;
		
		state_tq->header.type = PARSETREE_STATE_TYPE_AND_QUALIFIER;

		if (state->type == PARSETREE_STATE_TYPE) {
			ParseTreeState_TYPE *tstate = (ParseTreeState_TYPE*)state;
			state_tq->type = tstate->type;
			state_tq->qualifier = qualifier;
		} else if (state->type == PARSETREE_STATE_TYPE_AND_QUALIFIER) {
			ParseTreeState_TYPE_AND_QUALIFIER *tqstate = (ParseTreeState_TYPE_AND_QUALIFIER*)state;
			state_tq->type = tqstate->type;

			Qualifier elements[] = { tqstate->qualifier, qualifier };

			state_tq->qualifier = UnionQualifier_create((Array) {
				.data = elements, .size = 2
			}, this->state_alc);
		}

		//ParseTree_pushOption(this, opt);
		ParseTree_dispatch(this, token + 1, (ParseTreeState*)state_tq);
	}
}
