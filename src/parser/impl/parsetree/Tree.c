bool ParseTree_extendStateByNode(ParseTree *this, ParseTreeState **statep, Node node) {
	ParseTreeState *state = *statep;

	switch (state->type) {
		case PARSETREE_STATE_NONE:;
			goto discard_previous;
		default: goto extend_previous;
	}

	if (0) extend_previous: {
		ParseTreeState_EXTEND_NODE *state_ext =
			ParseTree_stalloc(this, sizeof(ParseTreeState_EXTEND_NODE));
		state_ext->state_node.header.type = PARSETREE_STATE_EXTEND_NODE;
		state_ext->state_node.node = node;
		state_ext->previous = state;
		*statep = (ParseTreeState*)state_ext;
	}

	if (0) discard_previous: {
		ParseTreeState_NODE *state_node =
			ParseTree_stalloc(this, sizeof(ParseTreeState_NODE));
		state_node->header.type = PARSETREE_STATE_NODE;
		state_node->node = node;
		*statep = (ParseTreeState*)state_node;
	}
	
	return true;
}

ParseTreeOption *ParseTree_evaluateOptions(ParseTree *this, Token *here, Array options) {
	if (options.size == 0) {
		return NULL;
	}

	ptrdiff_t best_rating = 0;
	ParseTreeOption *best_option = NULL;

	ParseTreeOption **it = options.data;
	ParseTreeOption **end = it + options.size;

	for (; it < end; it++) {
		ParseTreeOption *option = *it;
		ptrdiff_t rating = option->next_token - here;

		if (rating > best_rating) {
			best_rating = rating;
			best_option = option;
		}
	}

	return best_option;
}

bool ParseTree_dispatchOption(ParseTree *this, TokenStream *ts, ParseTreeState **statep, ParseTreeOption *option) {
	TokenStream_set(ts, option->next_token);

	switch (option->type) {
		case PARSETREE_OPTION_STATE: goto handle_state;
		case PARSETREE_OPTION_SUB: goto handle_sub;
	}

	if (0) handle_state: {
		ParseTreeOption_State *state_opt = (ParseTreeOption_State*)option;
		*statep = (ParseTreeState*)state_opt->state;
	}

	if (0) handle_sub: {
		ParseTreeOption_Sub *sub_opt = (ParseTreeOption_Sub*)option;
		if (!sub_opt->subf(this, ts, statep, sub_opt->payload)) return false;
	}

	return true;
}

bool ParseTree_resolveState(ParseTree *this, TokenStream *ts, ParseTreeState **statep) {
	begin:;

	Token *here = TokenStream_probe(ts);
	ParseTreeState *state = *statep;
	switch (state->type) {
		case PARSETREE_STATE_NODE: goto handle_node;
		case PARSETREE_STATE_EXTEND_NODE: goto handle_ext_node;
		default:;
	}

	if (0) handle_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		if (Node_isValueNode(state_node->node)) goto unwrap_node;
		goto eval_node;
	}

	if (0) handle_ext_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		if (Node_isValueNode(state_node->node)) goto quit;
		goto eval_node;
	}

	if (0) eval_node: {
		if (this->flags & PARSENODE_FLAG_NO_EVALUATE) goto quit;

		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;

		int eval_flags = ParserNode_eval_flags(state_node->node, this->ctx);

		if (eval_flags & PARSERNODE_EVAL_FLAG_IMPOSSIBLE) {
			if (this->flags & PARSENODE_FLAG_MUST_EVALUATE) {
				Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Cannot evaluate expression at parsetime (MUST_EVALUATE)", this->result);
				return false;
			}
			return true;
		}
		
		if (eval_flags & PARSERNODE_EVAL_FLAG_STACK) {
			if (!(this->flags & PARSENODE_FLAG_MUST_EVALUATE)) return true;
		}

		ParserResult result = ParserResult_NULL;
		ParserNode_evaluate(state_node->node, eval_flags, this->ctx, &result);
		if (Parser_checkfwd(&result, this->result)) return false;

		state_node->node = result.node;
		goto begin;
	}

	if (0) unwrap_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		ValueNode *val = state_node->node.object;

		if (Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) {
			ParseTreeState_TYPE *state_type = ParseTree_stalloc(this, sizeof(ParseTreeState_TYPE));
			state_type->header.type = PARSETREE_STATE_TYPE;
			state_type->type = *(Type*)val->data;
			*statep = (ParseTreeState*)state_type;
			return true;
		}
	}

	quit:;
	return true;
}

void ParseTree_stateToNode(
	ParseTree *this, ParseTreeState *state, ParserResult *out
) {
	switch (state->type) {
		case PARSETREE_STATE_NONE: goto handle_none;
		case PARSETREE_STATE_NODE: goto handle_node;
		case PARSETREE_STATE_MEMBER: goto handle_member;
		case PARSETREE_STATE_TYPE: goto handle_type;
		case PARSETREE_STATE_QUALIFIER: goto handle_qualifier;
		//case PARSETREE_STATE_IDENTIFIER: goto handle_identifier;
		case PARSETREE_STATE_DECLARATION: goto handle_declaration;
		default: goto err_invalid_state;
	}

	if (0) handle_none: {}

	if (0) handle_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		out->node = state_node->node;
	}

	if (0) handle_member: {
		ParseTreeState_MEMBER *state_member = (ParseTreeState_MEMBER*)state;
		Member *member = state_member->member;

		#ifdef BUILD_DEBUG
		PrintFmt(this->ctx->dbgstream, "stn handle_member {}\n", Type_repr(this->result->expect));
		#endif

		if (Type_equalPrimitive(Type_strip(this->result->expect), PRIMITIVE_TYPE_MEMBER)) {
			#ifdef BUILD_DEBUG
			OutStream_puts(this->ctx->dbgstream, "stn as member\n");
			#endif
			out->node = ValueNode_create(
				PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER),
				(char*)&member, this->ctx->program_alc
			);
		} else {
			out->node = GetNode_create(member, this->ctx->program_alc);
		}
	}

	if (0) handle_type: {
		ParseTreeState_TYPE *state_type = (ParseTreeState_TYPE*)state;
		Type type = Type_copy(state_type->type, this->ctx->program_alc);
		out->node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
			(char*)&type, this->ctx->program_alc
		);
	}

	if (0) handle_qualifier: {
		ParseTreeState_QUALIFIER *state_qual = (ParseTreeState_QUALIFIER*)state;
		Qualifier qualifier = Qualifier_copy(state_qual->qualifier, this->ctx->program_alc);
		out->node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_QUALIFIER),
			(char*)&qualifier, this->ctx->program_alc
		);
	}

	if (0) handle_identifier: {

	}

	if (0) handle_declaration: {
		if (!(this->flags & PARSENODE_FLAG_ALLOW_INTRIN)) goto err_no_intrin;

		ParseTreeState_DECLARATION *state_decl = (ParseTreeState_DECLARATION*)state;
		ParserIntrin_DECLARATION *val = Allocator_malloc(
			this->ctx->tmp_alc, sizeof(ParserIntrin_DECLARATION)
		);

		val->header.type = PARSER_INTRIN_DECLARATION;
		val->qualifier = state_decl->info.qualifier;
		val->type = state_decl->info.type;
		val->identifier = state_decl->info.identifier;

		out->node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_PARSER_INTRIN),
			(char*)&val, this->ctx->tmp_alc
		);
	}
	
	if (0) err_invalid_state: {
		Parser_throws(this->ctx, NULL, PARSER_RESULT_PANIC, "Cannot resolve result for parse tree state", out);
	}

	if (0) err_no_intrin: {
		Parser_throws(this->ctx, NULL, PARSER_RESULT_PANIC, "Cannot export parser intrinsic state", out);
	}
}

void ParseTree_parseNode(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	Vector optionStack; Vector_create(&optionStack, sizeof(ParseTreeOption*));
	Vector_init(&optionStack, 8, ctx->tmp_alc);

	ParseTree this = {
		.ctx = ctx,
		.options = { .vec = &optionStack, ctx->tmp_alc },
		.result = out,
		.state_alc = ctx->tmp_alc,
		.flags = out->flags,
		.done = false
	};

	ParseTreeState state_none = {
		.type = PARSETREE_STATE_NONE
	};

	ParseTreeState *state = &state_none;
	
	Token *token = TokenStream_probe(ts);

	#ifdef BUILD_DEBUG
	OutStream_indent(ctx->dbgstream, true);
	PrintFmt(ctx->dbgstream, "---- parseNode: {}\n", Token_repr(TokenStream_probe(ts)));
	#endif

	while (!this.done) {
		if (!ParseTree_resolveState(&this, ts, &state)) goto err_resolve_state;

		token = TokenStream_probe(ts);
		
		#ifdef BUILD_DEBUG
		PrintFmt(ctx->dbgstream, "march: {} {}\n",
			strrepr(EParseTreeStateType_REPR[state->type]),
			Token_repr(token)
		);
		#endif

		switch (token->type) {
			case TOKEN_TYPE_END: {
				if (this.flags & PARSENODE_FLAG_NO_EXPLICIT_END) goto err_no_explicit_end;
				if (!(this.flags & PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END)) {
					TokenStream_next(ts);
				}
				goto handle_end;
			}

			case TOKEN_TYPE_INPUT_END:
			case TOKEN_TYPE_LIST_DELIMITER:
			case TOKEN_TYPE_BRACE_CLOSE:
			case TOKEN_TYPE_SBRACE_CLOSE:
			case TOKEN_TYPE_CBRACE_CLOSE:
				goto handle_end;
			default:;
		}

		ParseTree_dispatch(&this, token, state);

		ParseTreeOption *option = ParseTree_evaluateOptions(&this, token, Vector_array(&optionStack));
		if (!option) break;

		if (!ParseTree_dispatchOption(&this, ts, &state, option)) goto handle_error;

		if (this.flags & PARSENODE_FLAG_NO_MARCH) break;
		if (0) handle_end: break;
	}

	token = TokenStream_probe(ts);

	#ifdef BUILD_DEBUG
	PrintFmt(ctx->dbgstream, "resolve: {}\n",
		strrepr(EParseTreeStateType_REPR[state->type]),
	);
	if (state->type == PARSETREE_STATE_NODE) {
		PrintFmt(ctx->dbgstream, "resolve_node: {}\n",
			Node_repr(((ParseTreeState_NODE*)state)->node)
		);
	}
	#endif

	if (state->type == PARSETREE_STATE_EXTEND_NODE) {
		state->type = PARSETREE_STATE_NODE;
	}

	if (!ParseTree_resolveState(&this, ts, &state)) goto err_resolve_state;

	ParserResult result = ParserResult_NULL;
	ParseTree_stateToNode(&this, state, &result);

	if (Parser_checkfwd(&result, out)) goto handle_error;

	if (!Type_nullOrMatch(out->expect, Node_resultType(result.node, ctx->tmp_alc))) {
		goto err_type_mismatch;
	}
	
	out->node = result.node;

	#ifdef BUILD_DEBUG
	PrintFmt(ctx->dbgstream, "---- result: {}", Node_repr(out->node));
	OutStream_unindent(ctx->dbgstream, true);
	#endif
	
	if (0) err_type_mismatch: {
		#ifdef BUILD_DEBUG
		PrintFmt(ctx->dbgstream, "---- type mismatch: {} {}", Node_repr(out->node), Type_repr(out->expect));
		OutStream_unindent(ctx->dbgstream, true);
		#endif
		Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Parser result doesn't match expected type", out);
	}
	
	if (0) err_resolve_state: {
		Parser_throws(ctx, &token->src, PARSER_RESULT_PANIC, "Failed to resolve state", this.result);
	}

	if (0) err_no_explicit_end: {
		Parser_throws(this.ctx, &token->src, PARSER_RESULT_PANIC, "Explicit end not allowed here", this.result);
	}

	if (0) handle_error: goto cleanup;

	cleanup: {
		Vector_destroy(&optionStack, ctx->tmp_alc);
	}
}
