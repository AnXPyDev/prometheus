void ParseTree_dispatch(ParseTree *this, Token *token, ParseTreeState *state) {
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
