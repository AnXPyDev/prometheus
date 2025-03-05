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

	Node root = ConditionNode_create(
		BuiltinNode_create(&Sim_builtin_equals, (Array) { .size = 2, .data = (Node[]) {
			ValueNode_createInt(5, alc),
			ValueNode_createInt(1, alc)
		} }, alc),
		BuiltinNode_create(&Sim_builtin_printArgs, (Array) { .size = 1, .data = (Node[]) {
			ValueNode_createInt(1, alc),
		} }, alc),
		BuiltinNode_create(&Sim_builtin_printArgs, (Array) { .size = 1, .data = (Node[]) {
			ValueNode_createInt(0, alc)
		} }, alc),
		alc
	);

	SimValue result = SimNode_evaluate(root, &context);

	PrintFmt(g_os_stdout, "{}\n", SimValue_repr(&result));

	ArenaAllocator_destroy(&arena);

	return 0;
}
