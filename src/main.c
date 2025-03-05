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
			ValueNode_createInt(5, alc)
		} }, alc),
		SequenceNode_create((Array) { .data = (Node[]) {
			ControlNode_create(SIM_CONTROL_SIGNAL_THROW, NULL, ValueNode_createInt(1337, alc), alc),
			ValueNode_createInt(1, alc),
		}, .size = 2 }, alc),
		BuiltinNode_create(&Sim_builtin_printArgs, (Array) { .size = 1, .data = (Node[]) {
			ValueNode_createInt(0, alc)
		} }, alc),
		alc
	);

	SimResult result;
	SimNode_evaluate(root, &context, &result);

	if (result.control) {
		PrintFmt(g_os_stdout, "interrupt: {%p} -> {%p} {}", repr(void*, result.control_origin), repr(void*, result.control_target), strrepr(ESimControlSignal_REPR[result.control]));
	}

	PrintFmt(g_os_stdout, "result: {}\n", SimValue_repr(&result.value));

	ArenaAllocator_destroy(&arena);

	return 0;
}
