void Language_setupBuiltinType(ParserFrame *frame, Allocator alc, StringView id, Type T) {
	*(Type*)ParserFrame_setupMemberWithValue(
		frame, id, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	) = T;
}

void Language_setupBuiltinQualifier(ParserFrame *frame, Allocator alc, StringView id, Qualifier Q) {
	*(Qualifier*)ParserFrame_setupMemberWithValue(
		frame, id, PrimitiveType_upcast(PRIMITIVE_TYPE_QUALIFIER)
	) = Q;
}

void Language_setupBuiltinFunction(ParserFrame *frame, Allocator alc, StringView id, Function *func) {
	*(FunctionValue*)ParserFrame_setupMemberWithValue(
		frame, id, func->type
	) = (FunctionValue) { .function = func, .closure = NULL };
}

void Language_setupBuiltins(ParserContext *ctx) {
	ParserFrame *frame = ctx->frame;
	Allocator alc = ctx->program_alc;
	MemberListAllocator *mla = ctx->state->mla;

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
		strview("Size"), PrimitiveType_upcast(PRIMITIVE_TYPE_SIZE)
	);
	Language_setupBuiltinType(frame, alc,
		strview("i32"), PrimitiveType_upcast(PRIMITIVE_TYPE_I32)
	);
	Language_setupBuiltinType(frame, alc,
		strview("u32"), PrimitiveType_upcast(PRIMITIVE_TYPE_U32)
	);
	Language_setupBuiltinType(frame, alc,
		strview("i8"), PrimitiveType_upcast(PRIMITIVE_TYPE_I8)
	);
	Language_setupBuiltinType(frame, alc,
		strview("u8"), PrimitiveType_upcast(PRIMITIVE_TYPE_U8)
	);
	Language_setupBuiltinType(frame, alc,
		strview("ByteSpan"), TYPE_BYTESPAN
	);
	Language_setupBuiltinType(frame, alc,
		strview("@void"), TYPE_VOIDPTR
	);
	
	Language_setupBuiltinQualifier(frame, alc,
		strview("const"), PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_CONSTANT)
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
		strview("add"), Language_builtin_sum_int_toFunction(mla, alc)
	);

	Language_setupBuiltinFunction(frame, alc,
		strview("neg"), Language_builtin_neg_int_toFunction(mla, alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("@"), Language_builtin_ptrtype_toFunction(mla, alc)
	);

}
