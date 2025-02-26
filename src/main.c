#include <ccore/ccore.h>

#include "include.h"

int main(int argc, char **argv) {
	g_initStdStreams();

	Member m1 = {
		.identifier = Identifier_new(strview("foo")),
		.qualifier = Qualifier_NULL,
		.type = PrimitiveType_upcast(PRIMITIVE_TYPE_INT)
	};

	QualifierType Q1 = QualifierType_new(PrimitiveQualifier_upcast(PRIMITIVE_QUALIFIER_CONSTANT), PrimitiveType_upcast(PRIMITIVE_TYPE_INT));
	
	Member m2 = {
		.identifier = Identifier_new(strview("bar")),
		.qualifier = Qualifier_NULL,
		.type = QualifierType_upcast(&Q1)
	};

	FunctionType FT = FunctionType_new(QualifierType_upcast(&Q1), PrimitiveType_upcast(PRIMITIVE_TYPE_INT));

	Member m3 = {
		.identifier = Identifier_new(strview("+")),
		.qualifier = Qualifier_NULL,
		.type = FunctionType_upcast(&FT)
	};

	Frame frame; Frame_create(&frame, g_standardAllocator);

	MemberList_add(&frame.ml, &m1);
	MemberList_add(&frame.ml, &m2);
	MemberList_add(&frame.ml, &m3);

	PrintFmt(g_os_stdout, "{}\n", Frame_repr(&frame));

	TypeInfo info; Type_info(QualifierType_upcast(&Q1), &info);
	PrintFmt(g_os_stdout, "{}\n", TypeInfo_repr(&info));


	/*
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
	*/
	return 0;
}
