void Language_builtin_takeptr_(Array args, SimContext *ctx, SimResult *out) {
	if (args.size == 0) return;
	
	SimValue *arg = args.data;
	if (!Type_match(arg->type, PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER))) {
		SimResult_throwMessage("takeptr: arg is not member", NULL, ctx, out);
	}

	Member *member = (Member*)arg->data;

	Type T = PointerType_create(member->type, ctx->state->temp_alc);

	void *ptr = SimStackFrame_getValue(ctx->frame, member);

	out->value = SimValue_create(
		&ptr, T,
		ctx->temp_alc
	);
}

Sim_builtin_fn_t Language_builtin_takeptr = &Language_builtin_ptrtype_;

Function *Language_builtin_takeptr_toFunction(MemberListAllocator *mla, Allocator alc) {
	MemberList *args = MemberList_create(mla);
	Member *arg_val = MemberList_add(
		args, strview("M"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_MEMBER)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_takeptr,
			PointerType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_ANY), alc),
			(Array) { .size = 1, .data = (Node[]) {
				GetNode_create(arg_val, alc),
			} }
		, 0, alc)
	, alc);

	return func;
}
