void Language_builtin_ptrtype_(Array args, SimContext *ctx, SimResult *out) {
	if (args.size == 0) return;
	
	SimValue *arg = args.data;
	if (!Type_match(arg->type, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE))) {
		SimResult_throwMessage("ptrtype: arg is not type", NULL, ctx, out);
	}

	Type T = *(Type*)arg->data;

	Type result = PointerType_create(T, ctx->state->temp_alc);

	out->value = SimValue_create(
		&result, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
		ctx->temp_alc
	);
}

Sim_builtin_fn_t Language_builtin_ptrtype = &Language_builtin_ptrtype_;

Function *Language_builtin_ptrtype_toFunction(Allocator alc) {
	MemberList *args = MemberList_create(alc);
	Member *arg_val = MemberList_add(
		args, strview("T"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_ptrtype, PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE),
			(Array) { .size = 1, .data = (Node[]) {
				GetNode_create(arg_val, alc),
			} }
		, BUILTIN_NODE_FLAG_PURE, alc)
	, alc);

	return func;
}
