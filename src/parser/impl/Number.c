void Parser_parseNumber(BufferView str, ParserContext *ctx, ParserResult *out) {
	// TODO check expected type
	// TODO handle casting and floats
	// TODO handle errors

	char vstr[256];
	memcpy(vstr, str.data, str.size);
	vstr[str.size] = 0;
	int val = atoi(vstr);

	out->node = ValueNode_create(TYPE_INT, (char*)&val, ctx->state->program_alc);
}
