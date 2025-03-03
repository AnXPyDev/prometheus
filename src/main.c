#include <ccore/ccore.h>

#include "include.h"

int main(int argc, char **argv) {
	g_initStdStreams();
	g_SimNode_setup_extension();
	g_SimValue_setup_consts();

	SimState simstate = {
		.alc = g_standardAllocator,
		.is_in = g_is_stdin,
		.os_out = g_os_stdout,
		.os_err = g_os_stderr
	};

	ArenaAllocator arena;
	ArenaAllocator_create(&arena, simstate.alc, 2048);

	SimContext context = {
		.frame = NULL,
		.state = &simstate,
		.temp_alc = ArenaAllocator_upcast(&arena)
	};


	ValueNode *val = ValueNode_create(context.temp_alc,
		PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
		(char*)(&(int) { 255 })
	);

	Node args[] = { ValueNode_upcast(val), ValueNode_upcast(val) };

	Array arrArgs = toArray(args);
	
	BuiltinNode *print_node = BuiltinNode_create(&Sim_builtin_equals, arrArgs, context.temp_alc);
	

	SimValue result = SimNode_evaluate(BuiltinNode_upcast(print_node), &context);

	PrintFmt(g_os_stdout, "{}\n", SimValue_repr(&result));

	ArenaAllocator_destroy(&arena);

	return 0;
}
