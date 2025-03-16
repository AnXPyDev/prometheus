typedef struct {
	InStream stream;
	InputPosition position;
	Vector buffer;
	uint32_t lastLineLength;
	StringView name;
	Allocator alc;
} ParserInStream;

void ParserInStream_create(
	ParserInStream *this,
	StringView name,
	InStream stream,
	Allocator allocator
) {
	this->alc = allocator;

	Vector_create(&this->buffer, sizeof(ParserChar));

	this->name = name;
	this->stream = stream;
	this->position = (InputPosition) { .line = 1, .character = 0 };
	this->lastLineLength = UINT32_MAX;
}

void ParserInStream_destroy(ParserInStream *this) {
	Vector_destroy(&this->buffer, this->alc);
}

ParserChar ParserInStream_getc(ParserInStream *this) {
	ParserChar c;
	if (this->buffer.size > 0) {
		c = *(ParserChar*)Vector_pop(&this->buffer);
	} else {
		c = InStream_getc(this->stream);
	}

	if (g_Parser_CharFlags[(int)ParserChar_toChar(c)] & PARSER_CHAR_NEWLINE) {
		this->lastLineLength = this->position.character;
		this->position.line++;
		this->position.character = 0;
	}

	this->position.character++;

	return c;
}

void ParserInStream_ungetc(ParserInStream *this, ParserChar c) {
	*(ParserChar*)Vector_push(&this->buffer, this->alc) = c;
	
	if (g_Parser_CharFlags[(int)ParserChar_toChar(c)] & PARSER_CHAR_NEWLINE) {
		this->position.line--;
		this->position.character = this->lastLineLength;
	}
}

bool ParserInStream_end(ParserInStream *this) {
	if (this->buffer.size > 0) {
		return false;
	}
	return InStream_end(this->stream);
}
