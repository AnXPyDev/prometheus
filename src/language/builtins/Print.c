void Language_builtin_print_val_(SimValue *val, OutStream os) {
	char buf[256];

	if (Type_match(val->type, TYPE_BYTESPAN)) {
		const char * const *span = val->data;
		BufferView str = {
			.data = span[0],
			.size = (Size)(span[1] - span[0])
		};

		OutStream_write(os, str);
	} else if (Type_isPrimitive(val->type)) {
		EPrimitiveType T = Type_asPrimitive(val->type);
		const void *data = val->data;
		switch (T) {
			case PRIMITIVE_TYPE_INT: sprintf(buf, "%d", *(int*)data); goto print_buf;
			default: goto dump;
		}
	} else dump: {
		SimValue_print(val, os, BufferView_NULL);
	}

	if (0) print_buf: {
		OutStream_puts(os, buf);
	}
}

void Language_builtin_print_(Array args, SimContext *ctx, SimResult *out) {
	if (args.size == 0) return;

	OutStream os = ctx->state->os_out;

	SimValue *it = args.data;
	SimValue *end = it + args.size;

	for (; it < end - 1; it++) {
		Language_builtin_print_val_(it, os);
	}
	Language_builtin_print_val_(it, os);
}

Sim_builtin_fn_t Language_builtin_print = &Language_builtin_print_;

Function *Language_builtin_print_toFunction(Allocator alc) {
	MemberList *args = MemberList_create(alc);
	Member *arg_val = MemberList_add(
		args, strview("val"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_ANY)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_print, PrimitiveType_upcast(PRIMITIVE_TYPE_VOID),
			(Array) { .size = 1, .data = (Node[]) {
				GetNode_create(arg_val, alc),
			} }
		, 0, alc)
	, alc);

	return func;
}

void Language_builtin_printdump_(Array args, SimContext *ctx, SimResult *out) {
	if (args.size == 0) return;

	OutStream os = ctx->state->os_err;

	SimValue *it = args.data;
	SimValue *end = it + args.size;

	for (it = args.data; it < end - 1; it++) {
		SimValue_print(it, os, BufferView_NULL);
		OutStream_putc(os, ' ');
	}
	SimValue_print(it, os, BufferView_NULL);

	OutStream_putc(os, '\n');
}

Sim_builtin_fn_t Language_builtin_printdump = &Language_builtin_printdump_;

Function *Language_builtin_printdump_toFunction(Allocator alc) {
	MemberList *args = MemberList_create(alc);
	Member *arg_val = MemberList_add(
		args, strview("val"), Qualifier_NULL, PrimitiveType_upcast(PRIMITIVE_TYPE_ANY)
	);

	Function *func = Function_create(args,
		BuiltinNode_create(&Language_builtin_printdump, PrimitiveType_upcast(PRIMITIVE_TYPE_VOID),
			(Array) { .size = 1, .data = (Node[]) {
				GetNode_create(arg_val, alc),
			} }
		, 0, alc)
	, alc);

	return func;
}
