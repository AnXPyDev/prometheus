void Language_setupBuiltins(ParserFrame *frame, Allocator alc) {
	// void
	Type *v_void = Allocator_malloc(alc, sizeof(Type));
	*v_void = PrimitiveType_upcast(PRIMITIVE_TYPE_VOID);
	Type **v_void_p = Allocator_malloc(alc, sizeof(Type*));
	*v_void_p = v_void;

	*ParserFrame_setupMemberWithValue(
		frame, strview("void"), PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	) = v_void_p;

	// int
	Type *v_int = Allocator_malloc(alc, sizeof(Type));
	*v_int = PrimitiveType_upcast(PRIMITIVE_TYPE_INT);
	Type **v_int_p = Allocator_malloc(alc, sizeof(Type*));
	*v_int_p = v_int;

	*ParserFrame_setupMemberWithValue(
		frame, strview("int"), PrimitiveType_upcast(PRIMITIVE_TYPE_TYPE)
	) = v_int_p;
	
	// print
	{
		MemberList *f_print_args = MemberList_create(alc);
		Member *f_print_arg_val = MemberList_add(f_print_args, 
			strview("val"),
			Qualifier_NULL,
			PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
		);
		
		Function *f_print_i = Function_create(f_print_args,
			SequenceNode_create((Array) { .size = 2, .data = (Node[]) {
				BuiltinNode_create(&Sim_builtin_printArgs, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
					(Array) { .size = 1, .data = (Node[]) {
						GetNode_create(f_print_arg_val, alc),
					} }
				, alc),
				ControlNode_create(SIM_CONTROL_SIGNAL_RETURN, NULL,
					GetNode_create(f_print_arg_val, alc)
				, alc)
			} }, alc)
		, alc);

		Function **f_print_i_p = Allocator_malloc(alc, sizeof(Function**));
		*f_print_i_p = f_print_i;

		*ParserFrame_setupMemberWithValue(
			frame, strview("print"), f_print_i->type
		) = f_print_i_p;
	}

	// add
	{	
		MemberList *f_add_args = MemberList_create(alc);
		Member *f_add_arg_a = MemberList_add(f_add_args, 
			strview("a"),
			Qualifier_NULL,
			PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
		);
		
		Member *f_add_arg_b = MemberList_add(f_add_args, 
			strview("b"),
			Qualifier_NULL,
			PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
		);
		
		Function *f_add_i = Function_create(f_add_args,
			BuiltinNode_create(&Sim_builtin_sum_ints, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
				(Array) { .size = 2, .data = (Node[]) {
					GetNode_create(f_add_arg_a, alc),
					GetNode_create(f_add_arg_b, alc),
				} }
			, alc)
		, alc);

		Function **f_add_i_p = Allocator_malloc(alc, sizeof(Function**));
		*f_add_i_p = f_add_i;

		*ParserFrame_setupMemberWithValue(
			frame, strview("add"), f_add_i->type
		) = f_add_i_p;
	}
	
	// neg
	{	
		MemberList *f_neg_args = MemberList_create(alc);
		Member *f_neg_arg_a = MemberList_add(f_neg_args, 
			strview("a"),
			Qualifier_NULL,
			PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
		);
		
		Function *f_neg_i = Function_create(f_neg_args,
			BuiltinNode_create(&Sim_builtin_neg_int, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
				(Array) { .size = 1, .data = (Node[]) {
					GetNode_create(f_neg_arg_a, alc),
				} }
			, alc)
		, alc);

		Function **f_neg_i_p = Allocator_malloc(alc, sizeof(Function**));
		*f_neg_i_p = f_neg_i;

		*ParserFrame_setupMemberWithValue(
			frame, strview("neg"), f_neg_i->type
		) = f_neg_i_p;
	}
}
