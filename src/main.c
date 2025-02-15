#include <ccore/ccore.h>

#include "simulator/extensions/include.h"
#include "tree/include.h"
#include "parser/include.h"
#include "simulator/include.h"

int main(int argc, char **argv) {
	g_initStdStreams();
	g_Parser_setupCharLookupTable();

	ArenaAllocator arena;
	ArenaAllocator_create(&arena, g_standardAllocator, 512);

	return 0;

	Parser parser;
	Parser_create(&parser, ArenaAllocator_upcast(&arena));

	Vector tokens = Vector_new(g_standardAllocator, sizeof(Token));
	ParserInStream input;
	ParserInStream_create(&input, strview("stdin"), g_is_stdin, g_standardAllocator);

	Parser_tokenize(&parser, &input, &tokens);

	OutStream_puts(g_os_stdout, "Tokenizer output:\n");
	for (Token* it = Vector_begin(&tokens); it < (Token*)Vector_end(&tokens); it++) {
		PrintFmt(g_os_stdout, "{}\n", Token_repr(it));
	}
}
