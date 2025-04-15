typedef struct {
	OutStream os;
	uint32_t indent;
} IndentOutStream;

void IndentOutStream_create(IndentOutStream *this, OutStream os) {
	this->indent = 0;
	this->os = os;
}

#define this ((IndentOutStream*)vthis)

void IndentOutStream_putc(void *vthis, int c) {
	OutStream_putc(this->os, c);
	if (c == '\n') {
		for (uint32_t i = 0; i < this->indent; i++) {
			OutStream_putc(this->os, '\t');
		}
	}
}

#undef this

const IOutStream IOutStream_IndentOutStream = {
	.close = &OutStreamPtr_close,
	.flush = &OutStreamPtr_flush,
	.putc = &IndentOutStream_putc
};

bool OutStream_isIndentOutStream(OutStream this) {
	return this.interface == &IOutStream_IndentOutStream;
}

OutStream IndentOutStream_upcast(IndentOutStream *this) {
	return (OutStream) { .interface = &IOutStream_IndentOutStream, .object = this };
}

void OutStream_indent(OutStream vthis, bool newline) {
	if (!OutStream_isIndentOutStream(vthis)) return;
	IndentOutStream *this = vthis.object;
	this->indent++;
	if (newline) IndentOutStream_putc(this, '\n');
}

void OutStream_unindent(OutStream vthis, bool newline) {
	if (!OutStream_isIndentOutStream(vthis)) return;
	IndentOutStream *this = vthis.object;
	if (this->indent > 0) this->indent--;
	if (newline) IndentOutStream_putc(this, '\n');
}
