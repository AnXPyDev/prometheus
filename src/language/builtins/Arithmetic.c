// sum ints
void Language_builtin_add_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data + *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_sub_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data - *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_mul_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data * *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_div_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data / *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_mod_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data % *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_equal_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data == *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_notequal_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data != *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_lt_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data < *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_lte_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data <= *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_gt_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data > *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_gte_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data >= *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_and_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data && *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_or_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = *(int*)A[0].data || *(int*)A[1].data;
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_neg_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = -(*(int*)A[0].data);
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

void Language_builtin_not_int_(Array args, SimContext *ctx, SimResult *out) {
	SimValue *A = args.data;
	int result = !(*(int*)A[0].data);
	out->value = SimValue_create(&result, PrimitiveType_upcast(PRIMITIVE_TYPE_INT), ctx->temp_alc);
}

Function *Language_builtin_binop_toFunction(Type T, Sim_builtin_fn_t fn, MemberListAllocator *mla, Allocator alc) {
	Sim_builtin_fn_t *fnptr = Allocator_malloc(alc, sizeof(Sim_builtin_fn_t));
	*fnptr = fn;

	MemberList *args = MemberList_create(mla);
	Member *arg_a = MemberList_add(args, strview("a"), Qualifier_NULL, T);
	Member *arg_b = MemberList_add(args, strview("b"), Qualifier_NULL, T);

	Node nodes[] = {
		GetNode_create(arg_a, alc),
		GetNode_create(arg_b, alc)
	};

	Function *func = Function_create(args,
		BuiltinNode_create(fnptr, T,
			(Array) { .data = nodes, .size = 2 },
			BUILTIN_NODE_FLAG_PURE, alc
		), alc
	);

	return func;
}

Function *Language_builtin_unop_toFunction(Type T, Sim_builtin_fn_t fn, MemberListAllocator *mla, Allocator alc) {
	Sim_builtin_fn_t *fnptr = Allocator_malloc(alc, sizeof(Sim_builtin_fn_t));
	*fnptr = fn;

	MemberList *args = MemberList_create(mla);
	Member *arg_a = MemberList_add(args, strview("a"), Qualifier_NULL, T);

	Node nodes[] = {
		GetNode_create(arg_a, alc),
	};

	Function *func = Function_create(args,
		BuiltinNode_create(fnptr, T,
			(Array) { .data = nodes, .size = 1 },
			BUILTIN_NODE_FLAG_PURE, alc
		), alc
	);

	return func;
}
