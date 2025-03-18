typedef struct {
	Allocator program_alc;
	OutStream logstream;
	ParserFrame *root_frame;
} ParserState;

typedef struct {
	ParserState *state;
	ParserFrame *frame;
	Allocator tmp_alc;
} ParserContext;
