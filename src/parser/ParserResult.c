bool Parser_check(ParserResult *result) {
	return result->code >= PARSER_RESULT_PANIC;
}

bool Parser_checkfwd(ParserResult *result, ParserResult *out) {
	if (Parser_check(result)) {
		out->code = result->code;
		return true;
	}
	return false;
}

void Parser_throw(ParserContext *context, ObjectSource *source, EParserResultCode code, StringView message, ParserResult *out_result) {
	OutStream log = context->logstream;
	PrintFmt(log, "{} {}\n",
		source ? ObjectSource_repr(source) : strrepr("???"),
		strrepr(EParserResultCode_repr[code])
	);
	
	if (!BufferView_isNull(message)) {
		OutStream_puts(log, "   ");
		OutStream_write(log, message);
		OutStream_putc(log, '\n');
	}

	if (out_result) {
		out_result->code = code;
	}
}

void Parser_throws(ParserContext *ctx, ObjectSource *src, EParserResultCode code, const char *msg, ParserResult *out) {
	Parser_throw(ctx, src, code, strview(msg), out);
}
