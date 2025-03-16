typedef struct {
	ParserContext *ctx;

	Vector options;
	Allocator options_alc;
} ParseTree;

#define NODE(s) PARSETREE_NODE_##s

typedef enum {
	NODE(NONE) = 0,
	NODE(TYPE),
	NODE(IDENTIFIER),
	NODE(MEMBER),
	NODE(FUNCTION),
	NODE(VALUE),

	NODE(SUB_DECLARATION),
	NODE(SUB_NUMBER),
	NODE(SUB_PTRTYPE),
	NODE(SUB_TUPLE),
	NODE(SUB_BRACED),
	NODE(SUB_FRAME),
	NODE(SUB_CONTROL),
	NODE(SUB_CONDITION),
	NODE(SUB_CATCH),
	NODE(SUB_CALL),
	NODE(SUB_LOOP),
	NODE(SUB_PRINT),
	NODE(SUB_SET),
	NODE(_END)
} EParseTreeNode;

const char *EParseTreeNode_REPR[NODE(_END)] = {
	[NODE(NONE)] = "NONE",
	[NODE(TYPE)] = "TYPE",
	[NODE(IDENTIFIER)] = "IDENTIFIER",
	[NODE(MEMBER)] = "MEMBER",
	[NODE(FUNCTION)] = "FUNCTION",
	[NODE(VALUE)] = "VALUE",

	[NODE(SUB_DECLARATION)] = "SUB_DECLARATION",
	[NODE(SUB_NUMBER)] = "SUB_NUMBER",
	[NODE(SUB_PTRTYPE)] = "SUB_PTRTYPE",
	[NODE(SUB_TUPLE)] = "SUB_TUPLE",
	[NODE(SUB_BRACED)] = "SUB_BRACED",
	[NODE(SUB_FRAME)] = "SUB_FRAME",
	[NODE(SUB_CONTROL)] = "SUB_CONTROL",
	[NODE(SUB_CONDITION)] = "SUB_CONDITION",
	[NODE(SUB_CATCH)] = "SUB_CATCH",
	[NODE(SUB_CALL)] = "SUB_CALL",
	[NODE(SUB_LOOP)] = "SUB_LOOP",
	[NODE(SUB_PRINT)] = "SUB_PRINT",
	[NODE(SUB_SET)] = "SUB_SET",
};

typedef struct {
	EParseTreeNode type;
	union {
		BufferView str;
		Type type;
		BufferView identifier;
		Member *member;
		ESimControlSignal control;
		Array funcs;
		Node value;
		struct { Type type; BufferView id; } decl;
	} u;
} ParseTreeNode;

typedef struct {
	Token *next_token;
	ParseTreeNode node;
} ParseTreeOption;

void ParseTree_dispatch(ParseTree*, Token*, ParseTreeNode*);

void ParseTree_branch_number(ParseTree *this, Token *token, ParseTreeNode *node) {
	ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
	*opt = (ParseTreeOption) {
		.next_token = token + 1,
		.node = { .type = NODE(SUB_NUMBER), .u.str = token->str }
	};
}

void ParseTree_branch_type(ParseTree *this, Token *token, Type T, ParseTreeNode *node) {
	switch (node->type) {
		default:	return;
		case NODE(NONE):;
	}

	ParseTreeNode nnode = {
		.type = NODE(TYPE),
		.u.type = T
	};

	ParseTree_dispatch(this, token + 1, &nnode);
}

void ParseTree_branch_member(ParseTree *this, Token *token, Member *member, ParseTreeNode *node) {
	switch (node->type) {
		default:;
			return;
		case NODE(NONE):;
	}

	ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
	*opt = (ParseTreeOption) {
		.next_token = token + 1,
		.node = {
			.type = NODE(MEMBER),
			.u.member = member
		}
	};
}

void ParseTree_branch_keyword(ParseTree *this, Token *token, EParserKeyword kw, ParseTreeNode *node) {
	ESimControlSignal control;
	EParseTreeNode sub_type;

	switch (node->type) {
		case NODE(NONE): {
			switch (kw) {
				default:
					return;
				// control
				case PARSER_KEYWORD_RETURN:
					control = SIM_CONTROL_SIGNAL_RETURN;
					goto handle_control;
				case PARSER_KEYWORD_EMIT:
					control = SIM_CONTROL_SIGNAL_EMIT;
					goto handle_control;
				case PARSER_KEYWORD_BREAK:
					control = SIM_CONTROL_SIGNAL_BREAK;
					goto handle_control;
				case PARSER_KEYWORD_CONTINUE:
					control = SIM_CONTROL_SIGNAL_CONTINUE;
					goto handle_control;
				case PARSER_KEYWORD_JUMP:
					control = SIM_CONTROL_SIGNAL_JUMP;
					goto handle_control;
				case PARSER_KEYWORD_THROW:
					control = SIM_CONTROL_SIGNAL_THROW;
					goto handle_control;
				case PARSER_KEYWORD_EXIT:
					control = SIM_CONTROL_SIGNAL_EXIT;
					goto handle_control;
				
			
				// statements
				case PARSER_KEYWORD_IF:
					sub_type = NODE(SUB_CONDITION);
					goto handle_sub;
				case PARSER_KEYWORD_LOOP:
					sub_type = NODE(SUB_LOOP);
					goto handle_sub;
				case PARSER_KEYWORD_CATCH:
					sub_type = NODE(SUB_CATCH);
					goto handle_sub;
			}
			break;
		}
		case NODE(MEMBER): {
			switch (kw) {
				case PARSER_KEYWORD_SET:
					sub_type = NODE(SUB_SET);
					goto handle_sub;
				default:;
			}
			break;
		}
		default:;
	}

	if (0) handle_control: {
		ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
		*opt = (ParseTreeOption) {
			.next_token = token + 1,
			.node = {
				.type = NODE(SUB_CONTROL),
				.u.control = control
			}
		};
	}

	if (0) handle_sub: {
		ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
		*opt = (ParseTreeOption) {
			.next_token = token + 1,
			.node = { .type = sub_type, .u = node->u }
		};
	}
}

void ParseTree_dispatch_member(ParseTree *this, Token *token, MemberValuePair mvp, ParseTreeNode *node) {
	//PrintFmt(this->ctx->state->logstream, "dispatch member: {} = {%p}\n", Member_repr(mvp.member), repr(void*, mvp.value));

	if (!mvp.value) goto skipval;

	Type MT = mvp.member->type;
	if (Type_equalPrimitive(MT, PRIMITIVE_TYPE_TYPE)) {
		Type T = **(Type**)mvp.value;
		ParseTree_branch_type(this, token, T, node);
	}

	if (Type_equalPrimitive(MT, PRIMITIVE_TYPE_KEYWORD)) {
		EParserKeyword kw = *(EParserKeyword*)mvp.value;
		ParseTree_branch_keyword(this, token, kw, node);
	}

	skipval:;

	ParseTree_branch_member(this, token, mvp.member, node);
}

void ParseTree_branch_identifier(ParseTree *this, Token *token, BufferView id, ParseTreeNode *node) {
	switch (node->type) {
		case NODE(TYPE): goto handle_declaration;
		default:;
	}

	if (0) handle_declaration: {
		ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
		*opt = (ParseTreeOption) {
			.next_token = token + 1,
			.node = {
				.type = NODE(SUB_DECLARATION),
				.u.decl = {
					.type = node->u.type,
					.id = id
				}
			}
		};
		return;
	}
}

void ParseTree_branch_function(ParseTree *this, Token *token, Array funcs, ParseTreeNode *node) {
	switch (node->type) {
		default:;
			return;
		case NODE(NONE):;
	}

	if (funcs.size > 0) {
		ParseTreeNode nnode = {
			.type = NODE(FUNCTION),
			.u.funcs = funcs
		};
		ParseTree_dispatch(this, token + 1, &nnode);
	}
}

void ParseTree_dispatch_identifier(ParseTree *this, Token *token, ParseTreeNode *node) {
	//PrintFmt(this->ctx->state->logstream, "dispatch id: {}\n", bufrepr(token->str));
	BufferView id = token->str;
	Vector mvps; Vector_create(&mvps, sizeof(MemberValuePair));
	ParserFrame_find(this->ctx->frame, id, &mvps, this->ctx->tmp_alc);

	MemberValuePair *it = Vector_begin(&mvps);
	MemberValuePair *end = Vector_end(&mvps);

	Vector funcs; Vector_create(&funcs, sizeof(Function*));

	for (; it < end; it++) {
		ParseTree_dispatch_member(this, token, *it, node);

		if (it->value && Type_isFunctionType(it->member->type)) {
			*(Function**)Vector_push(&funcs, this->ctx->tmp_alc) = *(Function**)it->value;

		}
	}

	ParseTree_branch_function(this, token, Vector_array(&funcs), node);

	ParseTree_branch_identifier(this, token, id, node);
}

void ParseTree_branch_brace(ParseTree *this, Token *token, ParseTreeNode *node) {
	switch (node->type) {
		case NODE(NONE):
			goto handle_braced;
		case NODE(FUNCTION):
			goto handle_call;
		default:;
	}

	if (0) handle_call: {
		ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
		*opt = (ParseTreeOption) {
			.next_token = token + 1,
			.node = {
				.type = NODE(SUB_CALL),
				.u.funcs = node->u.funcs
			}
		};
	}

	if (0) handle_braced: {
		ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
		*opt = (ParseTreeOption) {
			.next_token = token + 1,
			.node = {
				.type = NODE(SUB_BRACED)
			}
		};
	}
}

void ParseTree_branch_cbrace(ParseTree *this, Token *token, ParseTreeNode *node) {
	switch (node->type) {
		default:;
			return;
		case NODE(NONE):;
	}

	ParseTreeOption *opt = Vector_push(&this->options, this->options_alc);
	*opt = (ParseTreeOption) {
		.next_token = token + 1,
		.node = {
			.type = NODE(SUB_FRAME)
		}
	};
}

void ParseTree_dispatch(ParseTree *this, Token *token, ParseTreeNode *node) {
	switch (token->type) {
		case TOKEN_TYPE_NUMERIC_LITERAL: 
			ParseTree_branch_number(this, token, node);
			break;
		case TOKEN_TYPE_IDENTIFIER:
			ParseTree_dispatch_identifier(this, token, node);
			break;
		case TOKEN_TYPE_BRACE_OPEN:
			ParseTree_branch_brace(this, token, node);
			break;
		case TOKEN_TYPE_CBRACE_OPEN:
			ParseTree_branch_cbrace(this, token, node);
			break;
		default:;
			return;
	}
}

void Parser_parseNode(int flags, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParseTree tree = {
		.ctx = ctx,
		.options_alc = ctx->tmp_alc,
	};

	Token *token = NULL;
	
	ParseTreeNode root_node = { .type = NODE(NONE) };

	Vector_create(&tree.options, sizeof(ParseTreeOption));
	Vector_init(&tree.options, 4, tree.options_alc);

	if (0) sub_control: {
		Parser_parseControl(root_node.u.control, ts, ctx, out);
		return;
	}

	if (0) sub_number: {
		ParserResult result = ParserResult_NULL;
		Parser_parseNumber(root_node.u.str, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;

		root_node.type = NODE(VALUE);
		root_node.u.value = result.node;
	}

	if (0) sub_declaration: {
		Member *member = MemberList_add(ctx->frame->memberlist, root_node.u.decl.id, Qualifier_NULL, root_node.u.decl.type);

		root_node.type = NODE(MEMBER);
		root_node.u.member = member;
	}

	if (0) sub_braced: {
		ParserResult result = ParserResult_NULL;
		Parser_parseNode(PARSENODE_FLAG_NO_EXPLICIT_END, ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;

		Token *here = TokenStream_probe(ts);

		while (true) {
			Token *token = TokenStream_next(ts);
			switch (token->type) {
				default:;
					Parser_throw(ctx, &here->src, PARSER_RESULT_PANIC, "Expected closing brace", out);
					return;
				case TOKEN_TYPE_BRACE_CLOSE:;
			}
			break;
		}

		root_node.type = NODE(VALUE);
		root_node.u.value = result.node;
	}

	if (0) sub_frame: {
		ParserResult result = ParserResult_NULL;
		Parser_parseFrame(ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;

		Token *here = TokenStream_probe(ts);

		while (true) {
			Token *token = TokenStream_next(ts);
			switch (token->type) {
				default:;
					Parser_throw(ctx, &here->src, PARSER_RESULT_PANIC, "Expected closing brace", out);
					return;
				case TOKEN_TYPE_CBRACE_CLOSE:;
			}
			break;
		}
		
		root_node.type = NODE(VALUE);
		root_node.u.value = result.node;
	}

	if (0) sub_call: {
		ParserResult result = ParserResult_NULL;
		Parser_parseCall(root_node.u.funcs, ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;
		
		Token *here = TokenStream_probe(ts);

		while (true) {
			Token *token = TokenStream_next(ts);
			switch (token->type) {
				default:;
					Parser_throw(ctx, &here->src, PARSER_RESULT_PANIC, "Expected closing brace for arglist", out);
					return;
				case TOKEN_TYPE_BRACE_CLOSE:;
			}
			break;
		}

		root_node.type = NODE(VALUE);
		root_node.u.value = result.node;
	}

	if (0) sub_set: {
		ParserResult result = ParserResult_NULL;
		Parser_parseNode(PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END, ts, ctx, &result);
		if (Parser_checkfwd(&result, out)) return;

		Node node = SetNode_create(root_node.u.member, result.node, ctx->state->program_alc);

		root_node.type = NODE(VALUE);
		root_node.u.value = node;
	}

	while (true) {
		switch (root_node.type) {
			case NODE(SUB_CONTROL): goto sub_control;
			case NODE(SUB_NUMBER): goto sub_number;
			case NODE(SUB_DECLARATION): goto sub_declaration;
			case NODE(SUB_BRACED): goto sub_braced;
			case NODE(SUB_FRAME): goto sub_frame;
			case NODE(SUB_CALL): goto sub_call;
			case NODE(SUB_SET): goto sub_set;
			default:;
		}

		Vector_clear(&tree.options);
		token = TokenStream_probe(ts);

		switch (token->type) {
			case TOKEN_TYPE_END: {
				if (flags & PARSENODE_FLAG_NO_EXPLICIT_END) {
					goto err_explicit_end;
				}
				if (!(flags & PARSENODE_FLAG_NO_CONSUME_EXPLICIT_END)) {
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


		ParseTree_dispatch(&tree, token, &root_node);

		ParseTreeOption *opt = NULL;

		{
			int max_rating = 0;
			ParseTreeOption *it = Vector_begin(&tree.options);
			ParseTreeOption *end = Vector_end(&tree.options);
			for (; it < end; it++) {
				int rating = (int)(it->next_token - token);
				#ifdef BUILD_DEBUG
				fprintf(stderr, "option: %s, %d\n", EParseTreeNode_REPR[it->node.type], rating);
				#endif
				if (rating > max_rating) {
					max_rating = rating;
					opt = it;
				}
			}
			#ifdef BUILD_DEBUG
			fprintf(stderr, "-------\n");
			#endif
		}
		
		if (!opt) {
			Parser_throw(ctx, &token->src, PARSER_RESULT_PANIC, "Cannot parse expression", out);
			return;
		}

		TokenStream_set(ts, opt->next_token);

		root_node = opt->node;

		if (0) handle_end: break;
	}

	switch (root_node.type) {
		case NODE(VALUE):
			out->node = root_node.u.value;
			break;
		case NODE(MEMBER):
			out->node = GetNode_create(root_node.u.member, ctx->state->program_alc);
			break;
		default:
			goto err_incomplete;
	}

	if (0) err_incomplete: {
		if (token->type == TOKEN_TYPE_INPUT_END) {
			Parser_throw(ctx, &token->src, PARSER_RESULT_ERROR, "Unexpected EOF", out);
			return;
		} else {
			Parser_throw(ctx, &token->src, PARSER_RESULT_WARNING, "Incomplete expression", out);
			return;
		}
	}

	if (0) err_explicit_end: {
		Parser_throw(ctx, &token->src, PARSER_RESULT_ERROR, "Explicit end not allowed here", out);
		return;
	}
}

#undef NODE
