bool Parser_check(ParserResult *result) {
	return result->code >= PARSER_RESULT_PANIC;
}

bool Parser_checkfwd(ParserResult *result, ParserResult *out) {
	if (Parser_check(result)) {
		out->code = result->code;
		out->source = result->source;
		out->payload = result->payload;
		return true;
	}
	return false;
}

void Parser_throw(ParserContext *context, ObjectSource *source, EParserResultCode code, const char *message, ParserResult *out_result) {
	OutStream log = context->state->logstream;
	PrintFmt(log, "{} {}\n",
		source ? ObjectSource_repr(source) : strrepr("???"),
		strrepr(EParserResultCode_repr[code])
	);
	
	if (message) {
		OutStream_puts(log, "   ");
		OutStream_puts(log, message);
		OutStream_putc(log, '\n');
	}

	if (out_result) {
		out_result->code = code;
		out_result->source = source;
	}
}
