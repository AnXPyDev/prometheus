// sum ints
void Language_builtin_sum_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *it = args.data;
	SimValue *end = it + args.size;

	int i = 0;

	for (; it < end; it++) {
		i += *(int*)it->data;
	}

	out->value = SimValue_create(&i, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

Sim_builtin_fn_t Language_builtin_sum_int = &Language_builtin_sum_int_;

Function *Language_builtin_sum_int_toFunction(MemberListAllocator *mla, Allocator alc) {
	MemberList *args = MemberList_create(mla);
	
	Member *arg_a = MemberList_add(
		args, strview("a"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);
	
	Member *arg_b = MemberList_add(
		args, strview("b"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_sum_int, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
			(Array) { .size = 2, .data = (Node[]) {
				GetNode_create(arg_a, alc),
				GetNode_create(arg_b, alc),
			} }
		, BUILTIN_NODE_FLAG_PURE, alc)
	, alc);

	return func;
}


// negate int
void Language_builtin_neg_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *it = args.data;

	int i = 0 - *(int*)it->data;

	out->value = SimValue_create(&i, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

Sim_builtin_fn_t Language_builtin_neg_int = &Language_builtin_neg_int_;

Function *Language_builtin_neg_int_toFunction(MemberListAllocator *mla, Allocator alc) {
	MemberList *args = MemberList_create(mla);
	
	Member *arg_a = MemberList_add(
		args, strview("a"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_neg_int, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
			(Array) { .size = 1, .data = (Node[]) {
				GetNode_create(arg_a, alc),
			} }
		, BUILTIN_NODE_FLAG_PURE, alc)
	, alc);

	return func;
}
