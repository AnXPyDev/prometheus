typedef struct {
	Allocator alc;
	OutStream os_out;
	OutStream os_err;
	InStream is_in;
} SimState;

typedef struct {
	SimState *state;
	SimStackFrame *frame;
	// used for allocating temporary values in a stack frame
	// should be an arena allocator
	Allocator temp_alc;
} SimContext;

