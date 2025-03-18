void Parser_parseString(BufferView str, ParserContext *ctx, ParserResult *out) {
	// TODO check expected type
	// TODO handle casting and floats
	// TODO handle errors

	char *string = Allocator_malloc(ctx->state->program_alc, str.size);
	memcpy(string, str.data, str.size);

	char *bs[2] = { string, string + str.size };

	out->node = ValueNode_create(
		TYPE_BYTESPAN,
		(char*)bs, ctx->state->program_alc
	);
}
