void ParseTree_branch_keyword(
	ParseTree *this, Token *token, ParseTreeState *state, EParserKeyword kw
) {
	ESimControlSignal control_signal;

	switch (state->type) {
		case PARSETREE_STATE_NONE: {
			switch (kw) {
				default: return;

				// control
				case PARSER_KEYWORD_RETURN:
					control_signal = SIM_CONTROL_SIGNAL_RETURN;
					goto kw_control;
				case PARSER_KEYWORD_EMIT:
					control_signal = SIM_CONTROL_SIGNAL_EMIT;
					goto kw_control;
				case PARSER_KEYWORD_BREAK:
					control_signal = SIM_CONTROL_SIGNAL_BREAK;
					goto kw_control;
				case PARSER_KEYWORD_CONTINUE:
					control_signal = SIM_CONTROL_SIGNAL_CONTINUE;
					goto kw_control;
				case PARSER_KEYWORD_JUMP:
					control_signal = SIM_CONTROL_SIGNAL_JUMP;
					goto kw_control;
				case PARSER_KEYWORD_THROW:
					control_signal = SIM_CONTROL_SIGNAL_THROW;
					goto kw_control;
				case PARSER_KEYWORD_EXIT:
					control_signal = SIM_CONTROL_SIGNAL_EXIT;
					goto kw_control;

				// statements	
				case PARSER_KEYWORD_IF: goto stmt_condition;
				case PARSER_KEYWORD_LOOP: goto stmt_loop;
				case PARSER_KEYWORD_CATCH: goto stmt_catch;

				case PARSER_KEYWORD_GET_PTR: goto stmt_getptr;
				case PARSER_KEYWORD_TAKE_PTR: goto stmt_takeptr;

				case PARSER_KEYWORD_STRUCT: goto kw_struct;
			}
		} break;

		case PARSETREE_STATE_QUALIFIER: {
			switch (kw) {
				case PARSER_KEYWORD_STRUCT: goto kw_struct;
				default:;
			}
		} break;

		case PARSETREE_STATE_MEMBER_ELEMENT:
		case PARSETREE_STATE_MEMBER: {
			switch (kw) {
				case PARSER_KEYWORD_SET: goto kw_set_member;
				case PARSER_KEYWORD_SET_PTR: goto kw_setptr;
				case PARSER_KEYWORD_CAST: goto kw_cast;
				default:;
			}
		} break;

		case PARSETREE_STATE_DECLARATION: {
			switch (kw) {
				case PARSER_KEYWORD_SET: goto kw_set_declaration;
				default:;
			}
		} break;
		
		case PARSETREE_STATE_NODE_ELEMENT:
		case PARSETREE_STATE_NODE: {
			switch (kw) {
				case PARSER_KEYWORD_SET_PTR: goto kw_setptr;
				case PARSER_KEYWORD_CAST: goto kw_cast;
				default:;
			}
		} break;

		default:;
	}

	return;

	if (0) kw_control: ParseTree_branch_control(this, token, state, control_signal);
	if (0) kw_set_member: ParseTree_branch_setMember(this, token, state);
	if (0) kw_set_declaration: ParseTree_branch_setDeclaration(this, token, state);

	if (0) stmt_condition: ParseTree_branch_condition(this, token, state);
	if (0) stmt_loop: ParseTree_branch_loop(this, token, state);
	if (0) stmt_catch: ParseTree_branch_catch(this, token, state);
	if (0) stmt_closure: {}

	if (0) stmt_getptr: ParseTree_branch_getptr(this, token, state);
	if (0) stmt_takeptr: ParseTree_branch_takeptr(this, token, state);
	
	if (0) kw_setptr: ParseTree_branch_setptr(this, token, state);
	if (0) kw_cast: ParseTree_branch_cast(this, token, state);
	if (0) kw_struct: ParseTree_branch_struct(this, token, state);
}
