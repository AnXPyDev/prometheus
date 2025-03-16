#include <ccore/ccore.h>

#include "include.h"

Node ValueNode_createInt(int i, Allocator alc) {
	return ValueNode_create(
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
		(char*)&i,
		alc
	);
}

int main(int argc, char **argv) {
	g_initStdStreams();
	g_SimNode_setup_extension();
	g_SimValue_setup_consts();

	ArenaAllocator arena;
	ArenaAllocator_create(&arena, g_standardAllocator, 2048);
	Allocator alc = ArenaAllocator_upcast(&arena);

	SimState simstate = {
		.alc = g_standardAllocator,
		.temp_alc = alc,
		.is_in = g_is_stdin,
		.os_out = g_os_stdout,
		.os_err = g_os_stderr,
	};
	
	SimState_init(&simstate);

	SimContext context = {
		.frame = NULL,
		.state = &simstate,
		.temp_alc = alc
	};

	MemberList *f_sum_args = MemberList_create(alc);
	Member *f_sum_arg_x = MemberList_add(f_sum_args, 
		Identifier_create(strview("x"), alc),
		Qualifier_NULL,
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);
	
	Member *f_sum_arg_y = MemberList_add(f_sum_args, 
		Identifier_create(strview("y"), alc),
		Qualifier_NULL,
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Function *f_sum = Function_create(f_sum_args,
		SequenceNode_create((Array) { .size = 2, .data = (Node[]) {
			BuiltinNode_create(&Sim_builtin_printArgs, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
				(Array) { .size = 2, .data = (Node[]) {
					GetNode_create(f_sum_arg_x, alc),
					GetNode_create(f_sum_arg_y, alc)
				} }
			, alc),
			ControlNode_create(SIM_CONTROL_SIGNAL_RETURN, NULL,
				BuiltinNode_create(&Sim_builtin_sum_ints, PrimitiveType_upcast(PRIMITIVE_TYPE_VOID),
					(Array) { .size = 2, .data = (Node[]) {
						GetNode_create(f_sum_arg_x, alc),
						GetNode_create(f_sum_arg_y, alc)
					} }
				, alc)
			, alc)
		} }, alc)
	, alc);

	MemberList *f_add_args = MemberList_create(alc);
	Member *f_add_arg_ptr = MemberList_add(f_add_args, 
		Identifier_create(strview("ptr"), alc),
		Qualifier_NULL,
		PointerType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_INT), alc)
	);
	
	Member *f_add_arg_x = MemberList_add(f_add_args, 
		Identifier_create(strview("x"), alc),
		Qualifier_NULL,
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);
	
	Function *f_add = Function_create(f_add_args,
		SetPointerNode_create(
			GetNode_create(f_add_arg_ptr, alc),
			BuiltinNode_create(&Sim_builtin_sum_ints, PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
				(Array) { .size = 2, .data = (Node[]) {
					GetPointerNode_create(GetNode_create(f_add_arg_ptr, alc), alc),
					GetNode_create(f_add_arg_x, alc)
				} }
			, alc)
		, alc)
	, alc);

	MemberList *root_ml = MemberList_create(alc);

	Member *root_var_i = MemberList_add(root_ml, 
		Identifier_create(strview("i"), alc),
		Qualifier_NULL,
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	);

	Node root2 = FrameNode_create(root_ml,
		SequenceNode_create((Array) { .size = 3, .data = (Node[]) {
			SetNode_create(root_var_i, ValueNode_createInt(12, alc), alc),
			CallNode_create(f_add, 
				(Array) { .size = 2, .data = (Node[]) {
					TakePointerNode_create(root_var_i, alc),
					ValueNode_createInt(255, alc)
				} }
			, alc),
			BuiltinNode_create(&Sim_builtin_printArgs, PrimitiveType_upcast(PRIMITIVE_TYPE_VOID),
				(Array) { .size = 1, .data = (Node[]) {
					GetNode_create(root_var_i, alc),
				} }
			, alc)
		} }, alc)
	, alc);

	Node root = CallNode_create(f_sum, 
		(Array) { .size = 2, .data = (Node[]) {
			ValueNode_createInt(5, alc),
			ValueNode_createInt(10, alc)
		} }
	, alc);

	PrintFmt(g_os_stdout, "f_sum: {}\n", Node_repr(f_sum->node));
	PrintFmt(g_os_stdout, "f_add: {}\n", Node_repr(f_add->node));
	PrintFmt(g_os_stdout, "root: {}\n", Node_repr(root));
	PrintFmt(g_os_stdout, "root2: {}\n", Node_repr(root));

	SimResult result = SimResult_NULL;
	SimNode_evaluate(root2, &context, &result);

	if (result.control) {
		PrintFmt(g_os_stdout, "interrupt: {%p} -> {%p} {}\n", repr(void*, result.control_origin), repr(void*, result.control_target), strrepr(ESimControlSignal_REPR[result.control]));
	}

	PrintFmt(g_os_stdout, "result: {}\n", SimValue_repr(&result.value));

	ArenaAllocator_destroy(&arena);

	return 0;
}
