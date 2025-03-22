void Parser_parseFrame(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserFrame frame;
	ParserFrame_create(&frame, ctx->frame, MemberList_create(ctx->state->mla), ctx->tmp_alc);

	ParserContext nctx = *ctx;
	nctx.frame = &frame;	

	ParserResult result = ParserResult_NULL;

	Parser_parseSequence(ts, &nctx, &result);
	if (Parser_checkfwd(&result, out)) return;

	Node node = FrameNode_create(frame.memberlist, result.node, ctx->program_alc);

	{
		FrameNode *fnode = node.object;
		Array members = MemberList_members(frame.memberlist);
		Member **it = members.data;
		Member **end = it + members.size;
		for (; it < end; it++) {
			void *val = ParserFrame_getValue(&frame, *it);
			if (!val) continue;
			Size ts = Type_size((*it)->type);

			void *prgval = Allocator_malloc(ctx->program_alc, ts);
			memcpy(prgval, val, ts);

			fnode->values[(*it)->index] = prgval;
		}
	}

	out->node = node;
}

#define this ((FrameNode*)vthis)

int FrameNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->root, ctx);
}

#undef this

const IParserNode IParserNode_FrameNode = {
	.eval_flags = &FrameNode_ParserNode_eval_flags
};
