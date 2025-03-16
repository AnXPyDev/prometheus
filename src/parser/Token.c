typedef struct {
	ETokenType type;
	StringView str;
	ObjectSource src;
} Token;

#define this ((Token*)vthis)
void Printable_Token_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "{} \"{}\" = {}",
		ObjectSource_repr(&this->src),
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

void Token_create(Token *this, ETokenType type, StringView str, ObjectSource src) {
	this->src = src;
	this->type = type;
	this->str = str;
}

typedef struct {
	Token *token;
} TokenStream;

Token *TokenStream_next(TokenStream *this) {
	return this->token++;
}

Token *TokenStream_probe(TokenStream *this) {
	return this->token;
}

void TokenStream_set(TokenStream *this, Token *token) {
	this->token = token;
}
