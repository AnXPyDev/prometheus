void Language_setupBuiltinType(ParserFrame *frame, Allocator alc, StringView id, Type T) {
	Type *v = Allocator_malloc(alc, sizeof(Type));
	*v = T;

	*ParserFrame_setupMemberWithValue(
		frame, id, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	) = v;
}

void Language_setupBuiltinFunction(ParserFrame *frame, Allocator alc, StringView id, Function *func) {
	Function **v = Allocator_malloc(alc, sizeof(Function*));
	*v = func;
	*ParserFrame_setupMemberWithValue(
		frame, id, func->type
	) = v;
}

void Language_setupBuiltins(ParserFrame *frame, Allocator alc) {
	// void

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

	Language_setupBuiltinFunction(frame, alc,
		strview("print"), Language_builtin_print_toFunction(alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("printdump"), Language_builtin_printdump_toFunction(alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("add"), Language_builtin_sum_int_toFunction(alc)
	);

	Language_setupBuiltinFunction(frame, alc,
		strview("neg"), Language_builtin_neg_int_toFunction(alc)
	);
	
	Language_setupBuiltinFunction(frame, alc,
		strview("@"), Language_builtin_ptrtype_toFunction(alc)
	);

}
