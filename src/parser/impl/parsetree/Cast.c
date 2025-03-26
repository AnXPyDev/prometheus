bool ParseTree_sub_cast(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	ParserResult result = ParserResult_NULL;

	ParseTree_stateToNode(this, *statep, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Node node = result.node;

	result = ParserResult_NULL;
	result.flags = 
		PARSENODE_FLAG_NO_MARCH |
		PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END |
		PARSENODE_FLAG_MUST_EVALUATE;
	result.expect = PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE);

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	if (!Node_isValueNode(result.node)) goto err_not_type;
	ValueNode *val = result.node.object;
	if (!Type_equalPrimitive(val->T, PRIMITIVE_TYPE_TYPE)) goto err_not_type;
	Type T = *(Type*)val->data;

	Node cast = CastNode_create(T, node, this->ctx->program_alc);

	return ParseTree_extendStateByNode(this, statep, cast);

	if (0) err_not_type: {
		Token *here = TokenStream_probe(ts);
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Expected type value for casting", this->result);
		return false;
	}

	return true;
}

void ParseTree_branch_cast(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_cast;

	ParseTree_pushOption(this, opt);
}
