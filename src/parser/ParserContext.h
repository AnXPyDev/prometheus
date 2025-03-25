typedef struct {
	Allocator alc;
	MemberListAllocator *mla;
	ParserFrame *root_frame;
	bool error;
} ParserState;

typedef struct {
	ParserState *state;
	ParserFrame *frame;
	OutStream logstream;
	OutStream dbgstream;
	Allocator program_alc;
	Allocator tmp_alc;
} ParserContext;
