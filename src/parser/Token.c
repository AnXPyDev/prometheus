typedef struct {
	ETokenType type;
	StringView str;
	ParserInStream *source;
	InputPosition position;
} Token;

#define this ((Token*)vthis)
void Printable_Token_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "{}:{%u}:{%u} \"{}\" = {}",
		bufrepr(String_view(&this->source->name)),
		repr(unsigned int, this->position.line),
		repr(unsigned int, this->position.character),
		bufrepr(this->str),
		strrepr(ETokenType_repr[this->type])
	);
}
#undef this

const IPrintable IPrintable_Token = {
	.print = &Printable_Token_print
};

Printable Token_repr(Token *this) {
	return (Printable) { .object = (void*)this, .interface = &IPrintable_Token };
}
