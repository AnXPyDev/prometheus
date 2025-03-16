typedef struct {
	Allocator program_alc;
	OutStream logstream;
} ParserState;

typedef struct {
	ParserState *state;
	ParserFrame *frame;
	Allocator tmp_alc;
} ParserContext;
