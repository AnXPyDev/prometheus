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
		case PARSETREE_STATE_NONE: {
			/*
			#ifdef BUILD_DEBUG
			//OutStream_puts(this->ctx->logstream, "resolveState: STATE_NONE\n");
			#endif
			*/
		} break;
		case PARSETREE_STATE_NODE: {
			ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;

			/*
			#ifdef BUILD_DEBUG
			PrintFmt(this->ctx->logstream, "resolveState: STATE_NODE {}\n",
				Node_repr(state_node->node)
			);
			#endif
			*/

			if (Node_isValueNode(state_node->node)) goto unwrap_node;
			if (this->flags & PARSENODE_FLAG_NO_EVALUATE) break;
			goto eval_node;
		} break;
		default:;
	}

	if (0) eval_node: {
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

	return true;
}

bool ParseTree_resolveResult(ParseTree *this, Token *here, ParseTreeState *state) {
	ParserResult *out = this->result;

	Node node = Node_NULL;

	switch (state->type) {
		case PARSETREE_STATE_NONE: goto handle_none;
		case PARSETREE_STATE_NODE: goto handle_node;
		case PARSETREE_STATE_MEMBER: goto handle_member;
		case PARSETREE_STATE_TYPE: goto handle_type;
		case PARSETREE_STATE_QUALIFIER: goto handle_qualifier;
		case PARSETREE_STATE_IDENTIFIER: goto handle_identifier;
		case PARSETREE_STATE_DECLARATION: goto handle_declaration;
		default: goto err_invalid_state;
	}


	if (0) handle_none: {}

	if (0) handle_node: {
		ParseTreeState_NODE *state_node = (ParseTreeState_NODE*)state;
		node = state_node->node;
	}

	if (0) handle_member: {
		ParseTreeState_MEMBER *state_member = (ParseTreeState_MEMBER*)state;
		Member *member = state_member->member;

		if (Type_equalPrimitive(Type_strip(out->expect), PRIMITIVE_TYPE_MEMBER)) {
			node = ValueNode_create(
				PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER),
				(char*)&member, this->ctx->program_alc
			);
		} else {
			node = GetNode_create(member, this->ctx->program_alc);
		}
	}

	if (0) handle_type: {
		ParseTreeState_TYPE *state_type = (ParseTreeState_TYPE*)state;
		Type type = Type_copy(state_type->type, this->ctx->program_alc);
		node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
			(char*)&type, this->ctx->program_alc
		);
	}

	if (0) handle_qualifier: {

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

		node = ValueNode_create(
			PrimitiveType_upcast(PRIMITIVE_TYPE_PARSER_INTRIN),
			(char*)&val, this->ctx->tmp_alc
		);
	}

	check: {
		if (!Type_nullOrMatch(out->expect, Node_resultType(node, this->ctx->tmp_alc))) {
			goto err_type_mismatch;
		}
		out->node = node;
	}

	if (0) err_type_mismatch: {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Parser result doesn't match expected type", out);
		return false;
	}

	if (0) err_invalid_state: {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Cannot resolve result for parse tree state", out);
		return false;
	}

	if (0) err_no_intrin: {
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Cannot export parser intrinsic state", out);
		return false;
	}

	return true;
}

void ParseTree_parseNode(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	Vector optionStack; Vector_create(&optionStack, sizeof(ParseTreeOption*));
	Vector_init(&optionStack, 8, ctx->tmp_alc);

	ParseTree this = {
		.ctx = ctx,
		.options = { .vec = &optionStack, ctx->tmp_alc },
		.result = out,
		.state_alc = ctx->tmp_alc,
		.flags = out->flags
	};

	ParseTreeState state_none = {
		.type = PARSETREE_STATE_NONE
	};

	ParseTreeState *state = &state_none;

	while (true) {
		if (!ParseTree_resolveState(&this, ts, &state)) goto handle_error;

		Token *token = TokenStream_probe(ts);

		switch (token->type) {
			case TOKEN_TYPE_END: {
				if (this.flags & PARSENODE_FLAG_NO_EXPLICIT_END) {
					Parser_throws(this.ctx, &token->src, PARSER_RESULT_PANIC, "Explicit end not allowed here", this.result);
					goto handle_error;
				}
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

		// subroutine signaled to exit this function
		if (state == NULL) goto cleanup;

		if (this.flags & PARSENODE_FLAG_NO_MARCH) break;
		if (0) handle_end: break;
	}
	
	if (0) handle_error: goto cleanup;

	ParseTree_resolveResult(&this, TokenStream_probe(ts), state);

	cleanup: {
		Vector_destroy(&optionStack, ctx->tmp_alc);
	}
}
