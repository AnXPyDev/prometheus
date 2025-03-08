typedef struct {
	Allocator alc;
	Allocator temp_alc;

	OutStream os_out;
	OutStream os_err;
	InStream is_in;

	SimCache cache;
	SimStackFrame *root_frame;
} SimState;

typedef struct {
	SimState *state;
	SimStackFrame *frame;
	// used for allocating temporary values in a stack frame
	// should be an arena allocator
	Allocator temp_alc;
} SimContext;
