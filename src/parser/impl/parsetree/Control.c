bool ParseTree_sub_control(
	ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload
) {
	ESimControlSignal signal = *(ESimControlSignal*)payload;

	ParserResult result = ParserResult_NULL;
	Parser_parseControl(signal, ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	this->result->node = result.node;

	*statep = NULL;
	return true;
}

void ParseTree_branch_control(
	ParseTree *this, Token *token, ParseTreeState *state, ESimControlSignal signal
) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ESimControlSignal));

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_control;
	*(ESimControlSignal*)opt->payload = signal;

	ParseTree_pushOption(this, opt);
}
