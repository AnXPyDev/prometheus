bool ParseTree_sub_takeptr(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	ParserResult result = ParserResult_NULL;

	result = ParserResult_NULL;
	result.flags = 
		PARSENODE_FLAG_NO_MARCH |
		PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END;
	result.expect = PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER);

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	if (!Node_isValueNode(result.node)) goto err_wrong_type;
	ValueNode *val = result.node.object;
	if (!Type_equalPrimitive(val->T, PRIMITIVE_TYPE_MEMBER)) goto err_wrong_type;


	Node takeptr = TakePointerNode_create(*(Member**)val->data, this->ctx->program_alc);

	return ParseTree_extendStateByNode(this, statep, takeptr);

	if (0) err_wrong_type: {
		Token *here = TokenStream_probe(ts);
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Expected member value for taking ptr", this->result);
		return false;
	}
}

void ParseTree_branch_takeptr(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_takeptr;

	ParseTree_pushOption(this, opt);
}

bool ParseTree_sub_getptr(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	ParserResult result = ParserResult_NULL;

	result = ParserResult_NULL;
	result.flags = 
		PARSENODE_FLAG_NO_MARCH |
		PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END;

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	if (!Type_isPointerType(Node_resultType(result.node, this->ctx->tmp_alc))) goto err_wrong_type;

	Node takeptr = GetPointerNode_create(result.node, this->ctx->program_alc);

	return ParseTree_extendStateByNode(this, statep, takeptr);

	if (0) err_wrong_type: {
		Token *here = TokenStream_probe(ts);
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Expected pointer here", this->result);
		return false;
	}
}

void ParseTree_branch_getptr(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_getptr;

	ParseTree_pushOption(this, opt);
}

bool ParseTree_sub_setptr(ParseTree *this, TokenStream *ts, ParseTreeState **statep, void *payload) {
	ParseTreeState *state = *(ParseTreeState**)payload;

	ParserResult result = ParserResult_NULL;
	
	ParseTree_stateToNode(this, state, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Node node = result.node;

	Type NT = Type_strip(Node_resultType(node, this->ctx->tmp_alc));

	if (!Type_isPointerType(NT)) goto err_wrong_type;

	result = ParserResult_NULL;
	result.flags = PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END;

	result.expect = ((PointerType*)NT.object)->T;

	Parser_parseNode(ts, this->ctx, &result);
	if (Parser_checkfwd(&result, this->result)) return false;

	Node setptr = SetPointerNode_create(node, result.node, this->ctx->program_alc);

	return ParseTree_extendStateByNode(this, statep, setptr);

	if (0) err_wrong_type: {
		Token *here = TokenStream_probe(ts);
		Parser_throws(this->ctx, &here->src, PARSER_RESULT_PANIC, "Expected pointer here", this->result);
		return false;
	}
}

void ParseTree_branch_setptr(ParseTree *this, Token *token, ParseTreeState *state) {
	ParseTreeOption_Sub *opt = ParseTree_stalloc(this, sizeof(ParseTreeOption_Sub) + sizeof(ParseTreeState*));
	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_SUB;
	opt->subf = &ParseTree_sub_setptr;
	*(ParseTreeState**)opt->payload = state;

	ParseTree_pushOption(this, opt);
}
