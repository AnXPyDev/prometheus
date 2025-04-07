void Language_setupBuiltinType(ParserFrame *frame, Allocator alc, StringView id, Type T) {
	*(Type*)ParserFrame_setupMemberWithValue(
		frame, id, Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	) = T;
}

void Language_setupBuiltinQualifier(ParserFrame *frame, Allocator alc, StringView id, Qualifier Q) {
	*(Qualifier*)ParserFrame_setupMemberWithValue(
		frame, id, Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_QUALIFIER)
	) = Q;
}

void Language_setupBuiltinFunction(ParserFrame *frame, Allocator alc, StringView id, Function *func) {
	*(FunctionValue*)ParserFrame_setupMemberWithValue(
		frame, id, Qualifier_NULL, func->type
	) = (FunctionValue) { .function = func, .closure = NULL };
}

void Language_setupBuiltinBinop(ParserFrame *frame, Allocator alc, StringView id, Function *func) {
	*(FunctionValue*)ParserFrame_setupMemberWithValue(
		frame, id, PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_OPERATOR_BINARY), func->type
	) = (FunctionValue) { .function = func, .closure = NULL };
}

void Language_setupBuiltinUnop(ParserFrame *frame, Allocator alc, StringView id, Function *func) {
	*(FunctionValue*)ParserFrame_setupMemberWithValue(
		frame, id, PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_OPERATOR_UNARY), func->type
	) = (FunctionValue) { .function = func, .closure = NULL };
}

void Language_setupBuiltins(ParserContext *ctx) {
	ParserFrame *frame = ctx->frame;
	Allocator alc = ctx->program_alc;
	MemberListAllocator *mla = ctx->state->mla;

	Language_setupBuiltinType(frame, alc,
		strview("auto"), PrimitiveType_upcast(PRIMITIVE_TYPE_AUTO)
	);

	Language_setupBuiltinType(frame, alc,
		strview("any"), PrimitiveType_upcast(PRIMITIVE_TYPE_AUTO)
	);

	Language_setupBuiltinType(frame, alc,
		strview("type"), PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	);

	Language_setupBuiltinType(frame, alc,
		strview("void"), PrimitiveType_upcast(PRIMITIVE_TYPE_VOID)
	);

	Language_setupBuiltinType(frame, alc,
		strview("byte"), PrimitiveType_upcast(PRIMITIVE_TYPE_BYTE)
	);
	
	Language_setupBuiltinType(frame, alc,
		strview("int"), PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Language_setupBuiltinType(frame, alc,
		strview("bool"), PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Language_setupBuiltinType(frame, alc,
		strview("ByteSpan"), TYPE_BYTESPAN
	);

	Language_setupBuiltinQualifier(frame, alc,
		strview("_op_unary"), PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_OPERATOR_UNARY)
	);
	
	Language_setupBuiltinQualifier(frame, alc,
		strview("_op_binary"), PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_OPERATOR_BINARY)
	);

	Language_setupBuiltinFunction(frame, alc,
		strview("print"), Language_builtin_print_toFunction(mla, alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("printdump"), Language_builtin_printdump_toFunction(mla, alc)
	);

	Language_setupBuiltinFunction(frame, alc,
		strview("@"), Language_builtin_ptrtype_toFunction(mla, alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("sizeof"), Language_builtin_sizeof_toFunction(mla, alc)
	);

	Type INT = PrimitiveType_upcast(PRIMITIVE_TYPE_INT);

	Language_setupBuiltinBinop(frame, alc, strview("+"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_add_int_, mla, alc)
	);
	
	Language_setupBuiltinBinop(frame, alc, strview("-"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_sub_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("*"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_mul_int_, mla, alc)
	);
	
	Language_setupBuiltinBinop(frame, alc, strview("/"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_div_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("%"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_mod_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("=="),
		Language_builtin_binop_toFunction(INT, &Language_builtin_equal_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("!="),
		Language_builtin_binop_toFunction(INT, &Language_builtin_notequal_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("<"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_lt_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("<="),
		Language_builtin_binop_toFunction(INT, &Language_builtin_lte_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview(">"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_gt_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview(">="),
		Language_builtin_binop_toFunction(INT, &Language_builtin_gte_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("&&"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_and_int_, mla, alc)
	);

	Language_setupBuiltinBinop(frame, alc, strview("||"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_or_int_, mla, alc)
	);

	Language_setupBuiltinUnop(frame, alc, strview("neg"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_neg_int_, mla, alc)
	);
	
	Language_setupBuiltinUnop(frame, alc, strview("!"),
		Language_builtin_binop_toFunction(INT, &Language_builtin_not_int_, mla, alc)
	);

	*(int*)ParserFrame_setupMemberWithValue(frame, strview("true"), Qualifier_NULL, INT) = 1;
	*(int*)ParserFrame_setupMemberWithValue(frame, strview("false"), Qualifier_NULL, INT) = 0;

}
