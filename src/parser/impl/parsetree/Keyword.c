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
			}
		} break;

		case PARSETREE_STATE_MEMBER: {
			switch (kw) {
				case PARSER_KEYWORD_SET: goto kw_set_member;
				default:;
			}
		} break;

		case PARSETREE_STATE_DECLARATION: {
			switch (kw) {
				case PARSER_KEYWORD_SET: goto kw_set_declaration;
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
}
