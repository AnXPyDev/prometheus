void ParseTree_dispatch(ParseTree *this, Token *token, ParseTreeState *state) {
	#ifdef BUILD_DEBUG
	PrintFmt(this->ctx->dbgstream, "dispatch: {} {}\n",
		strrepr(EParseTreeStateType_REPR[state->type]),
		Token_repr(token)
	);
	#endif

	switch (state->type) {
		case PARSETREE_STATE_EXTEND_NODE: goto handle_extend_node;
		default:;
	}
			
	ValueNode *val;
	ParseTreeState *previous_state;

	if (0) handle_extend_node: {
		ParseTreeState_EXTEND_NODE *state_exn = (ParseTreeState_EXTEND_NODE*)state;
		previous_state = state_exn->previous;
		
		Node node = state_exn->state_node.node;

		#ifdef BUILD_DEBUG
		PrintFmt(this->ctx->dbgstream, "handle extend node {} prev: {}\n", Node_repr(node), strrepr(EParseTreeStateType_REPR[previous_state->type]));
		#endif


		if (Node_isValueNode(node)) {
			val = node.object;
			switch (Type_asPrimitive(val->T)) {
				case PRIMITIVE_TYPE_TYPE: goto branch_type;
				default:;
			}
		}

		goto discard_previous;
	}

	if (0) branch_type: {
		#ifdef BUILD_DEBUG
		OutStream_puts(this->ctx->dbgstream, "unwrap extend type\n");
		#endif
		Type T = *(Type*)val->data;
		ParseTree_branch_type(this, token - 1, previous_state, T);
		return;
	}

	if (0) discard_previous: {
		state->type = PARSETREE_STATE_NODE;
	}

	switch (token->type) {
		case TOKEN_TYPE_NUMERIC_LITERAL:
			ParseTree_branch_number(this, token, state);
			break;
		case TOKEN_TYPE_STRING_LITERAL:
			ParseTree_branch_string(this, token, state);
			break;
		case TOKEN_TYPE_IDENTIFIER:
			ParseTree_dispatch_identifier(this, token, state);
			break;
		case TOKEN_TYPE_BRACE_OPEN:
			ParseTree_branch_brace(this, token, state);
			break;
		case TOKEN_TYPE_SBRACE_OPEN:
			ParseTree_branch_index(this, token, state);
			break;
		case TOKEN_TYPE_CBRACE_OPEN:
			ParseTree_branch_cbrace(this, token, state);
			break;
		case TOKEN_TYPE_ACCESSOR:
			ParseTree_branch_access(this, token, state);
			break;

		case TOKEN_TYPE_END:
		case TOKEN_TYPE_INPUT_END:
		case TOKEN_TYPE_LIST_DELIMITER:
		case TOKEN_TYPE_BRACE_CLOSE:
		case TOKEN_TYPE_SBRACE_CLOSE:
		case TOKEN_TYPE_CBRACE_CLOSE:
			break;

		default: {
			Parser_throws(this->ctx, &token->src, PARSER_RESULT_PANIC, "ParseTree_dispatch: Unhandled token type", this->result);
			break;
		}
	}
}
