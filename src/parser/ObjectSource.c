typedef struct {
	ParserInStream *stream;
	InputPosition pos;
} ObjectSource;

#define this ((ObjectSource*)vthis)

void ObjectSource_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_write(os, this->stream->name);
	char buf[256];
	sprintf(buf, " %u:%u", this->pos.line, this->pos.character);
	OutStream_puts(os, buf);
}

#undef this

const IPrintable IPrintable_ObjectSource = {
	.print = &ObjectSource_print
};

Printable ObjectSource_repr(ObjectSource *this) {
	if (!this) return Printable_NULL;
	return (Printable) { .interface = &IPrintable_ObjectSource, .object = this };
}
