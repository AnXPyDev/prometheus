typedef struct {
	ParseTreeState header;
	BufferView identifier;	
} ParseTreeState_IDENTIFIER;

void ParseTree_branch_identifier(ParseTree *this, Token *token, ParseTreeState *state) {
	switch (state->type) {
		case PARSETREE_STATE_TYPE:
		case PARSETREE_STATE_TYPE_AND_QUALIFIER:
			ParseTree_branch_declaration(this, token, state);
			break;
		default:;
			return;
		case PARSETREE_STATE_NONE:;
	}

	ParseTreeOption_State *opt = ParseTree_stalloc(
		this, sizeof(ParseTreeOption_State) + sizeof(ParseTreeState_IDENTIFIER)
	);

	opt->header.next_token = token + 1;
	opt->header.type = PARSETREE_OPTION_STATE;

	ParseTreeState_IDENTIFIER *state_id = (ParseTreeState_IDENTIFIER*)opt->state;
	state_id->header.type = PARSETREE_STATE_IDENTIFIER;
	state_id->identifier = token->str;

	ParseTree_pushOption(this, opt);
}

void ParseTree_dispatch_identifier(ParseTree *this, Token *token, ParseTreeState *state) {
	#define MATCH_TYPE_PQUAL(T, Q) Qualifier_match(Type_getQualifier(T), PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_##Q))
	#define MATCH_PQUAL(MQ, Q) Qualifier_match(MQ, PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_##Q))

	#define IS_UNARY(M) MATCH_PQUAL((M)->qualifier, OPERATOR_UNARY)
	#define IS_BINARY(M) MATCH_PQUAL((M)->qualifier, OPERATOR_BINARY)

	Vector mvps; Vector_create(&mvps, sizeof(MemberValuePair));
	ParserFrame_find(this->ctx->frame, token->str, (Vector_Alc) { &mvps, this->ctx->tmp_alc });

	if (mvps.size == 0) goto skip_member;

	MemberValuePair *first = Vector_begin(&mvps);

	MemberValuePair *it = first;
	MemberValuePair *end = Vector_end(&mvps);

	Vector funcs; Vector_create(&funcs, sizeof(Parser_CallCandidate));

	Type FT = Type_strip(it->member->type);

	if (Type_isFunctionType(FT)) {
		if (state->type == PARSETREE_STATE_NONE && IS_UNARY(it->member)) goto op_unary;
		if (IS_BINARY(it->member)) goto op_binary;
		goto function;
	}

	if (0) op_unary: {
		for (; it < end; it++) {
			if (it->value && IS_UNARY(it->member)) {
				*(Parser_CallCandidate*)Vector_push(&funcs, this->ctx->tmp_alc)
					= (Parser_CallCandidate) {
						.ft = (FunctionType*)FT.object,
						.fv = (FunctionValue*)it->value
					};
			}
		}
		if (funcs.size == 0) goto skip_func;
		ParseTree_branch_operator_unary(this, token, state, Vector_array(&funcs));
	}

	if (0) op_binary: {
		for (; it < end; it++) {
			if (it->value && IS_BINARY(it->member)) {
				*(Parser_CallCandidate*)Vector_push(&funcs, this->ctx->tmp_alc)
					= (Parser_CallCandidate) {
						.ft = (FunctionType*)FT.object,
						.fv = (FunctionValue*)it->value
					};
			}
		}
		if (funcs.size == 0) goto skip_func;
		ParseTree_branch_operator_binary(this, token, state, Vector_array(&funcs));
	}

	if (0) function: {
		for (; it < end; it++) {
			if (it->value && !IS_BINARY(it->member) && !IS_UNARY(it->member)) {
				*(Parser_CallCandidate*)Vector_push(&funcs, this->ctx->tmp_alc)
					= (Parser_CallCandidate) {
						.ft = (FunctionType*)FT.object,
						.fv = (FunctionValue*)it->value
					};
			}
		}
		if (funcs.size == 0) goto skip_func;
		ParseTree_branch_function(this, token, state, Vector_array(&funcs));
	}

	skip_func:;

	ParseTree_dispatch_member(this, token, state, *first);

	skip_member:;
	ParseTree_branch_identifier(this, token, state);

	#undef IS_UNARY
	#undef IS_BINARY
	#undef MATCH_TYPE_PQUAL
	#undef MATCH_PQUAL
}
