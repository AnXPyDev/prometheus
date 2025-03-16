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
	g_Parser_setupCharLookupTable();

	ArenaAllocator arena;
	ArenaAllocator_create(&arena, g_standardAllocator, 2048);
	Allocator alc = ArenaAllocator_upcast(&arena);

	InStream in_stream = g_is_stdin;
	StringView in_name = strview("stdin");

	FileInStream fis = FileInStream_new(NULL);

	const char *fname = "./examples/decl.pth";

	if (argc > 1) {
		fname = argv[1];
	}

	if (fname) {
		FILE *f = fopen(fname, "r");
		if (!f) {
			fprintf(stderr, "Cannot open file %s\n", fname);
			goto quit;
		}

		fis = FileInStream_new(f);
		in_name = strview(fname);
		in_stream = FileInStream_upcast(&fis);
	}

	ParserInStream parser_in;

	ParserInStream_create(&parser_in, in_name, in_stream, alc);

	Vector tokens;
	Parser_tokenize(&parser_in, alc, &tokens, alc);

	#ifdef BUILD_DEBUG
	fprintf(stdout, "Tokens: %zu\n", tokens.size);

	for (Token *it = Vector_begin(&tokens), *end = Vector_end(&tokens); it < end; it++) {
		Printable_print(Token_repr(it), g_os_stdout, BufferView_NULL);
		OutStream_putc(g_os_stdout, '\n');
	}
	
	fprintf(stdout, "\n\n");
	#endif

	ParserState parser = {
		.logstream = g_os_stderr,
		.program_alc = alc
	};


	ParserFrame pframe;
	ParserFrame_create(&pframe, NULL, MemberList_create(alc), alc);

	Parser_setupBuiltins(&pframe, alc);
	Language_setupBuiltins(&pframe, alc);

	ParserContext parser_ctx = {
		.state = &parser,
		.frame = &pframe,
		.tmp_alc = alc
	};

	TokenStream ts = { .token = Vector_begin(&tokens) };

	ParserResult presult = ParserResult_NULL;

	Parser_parseSequence(&ts, &parser_ctx, &presult);

	if (Parser_check(&presult)) {
		fprintf(stderr, "Presult: %d\n", presult.code);
		goto quit;
	}

	PrintFmt(g_os_stdout, "root: {}\n", Node_repr(presult.node));

	SimState simstate = {
		.alc = g_standardAllocator,
		.temp_alc = alc,
		.is_in = g_is_stdin,
		.os_out = g_os_stdout,
		.os_err = g_os_stderr,
	};
	
	SimState_init(&simstate);


	SimContext context = {
		.state = &simstate,
		.temp_alc = alc
	};

	context.frame = SimStackFrame_create(NULL, pframe.memberlist, SimCache_getMemberList(&simstate.cache, pframe.memberlist), simstate.alc);

	SimResult result = SimResult_NULL;
	SimNode_evaluate(presult.node, &context, &result);

	if (result.control) {
		PrintFmt(g_os_stdout, "interrupt: {%p} -> {%p} {}\n", repr(void*, result.control_origin), repr(void*, result.control_target), strrepr(ESimControlSignal_REPR[result.control]));
	}

	PrintFmt(g_os_stdout, "result: {}\n", SimValue_repr(&result.value));

	if (Type_equalPrimitive(result.value.type, PRIMITIVE_TYPE_MESSAGE)) {
		PrintFmt(g_os_stdout, "message: {}\n", strrepr(*(const char**)result.value.data));
	}

	quit:;

	ArenaAllocator_destroy(&arena);
	InStream_close(in_stream);

	return 0;
}
