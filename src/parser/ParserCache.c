typedef struct {
	HashMap *map;
	Allocator alc;
	StringOutStream log;
} ParserCache;

typedef struct {
	TokenStream *ts;
	Token *token;
	ParserContext *ctx;

	ParserResult result;
	Buffer log;
	Token *token_after;
} ParserCache_Item;

ParserCache_Item *ParserResult_getCacheItem(ParserResult *result) {
	return (ParserCache_Item*)((char*)result - offsetof(ParserCache_Item, result));
}

void ParserCache_create(ParserCache *this, Allocator alc) {
	this->map = HashMap_create(32, alc);
	this->alc = alc;
	StringOutStream_create(&this->log, alc);
}

void ParserCache_export(TokenStream *ts, ParserContext *ctx, ParserResult *from, ParserResult *out) {
	ParserCache_Item *item = ParserResult_getCacheItem(from);

	out->code = from->code;
	out->node = Node_copy(from->node, ctx->program_alc);

	OutStream_write(ctx->logstream, Buffer_view(item->log));
	TokenStream_set(ts, item->token_after);
}

void ParserCache_destroy(ParserCache *this) {
	// TODO implement
}

bool ParserCache_matchItem(void *object, void *payload) {
	ParserCache_Item *item = object;
	ParserCache_Item *other = payload;

	return (
		(item->ts == other->ts) &&
		(item->result.flags == other->result.flags) &&
		(item->token == other->token) &&
		(item->ctx == other->ctx) &&
		Type_equal(item->result.expect, other->result.expect)
	);
}

Hash ParserCache_hashItem(ParserCache_Item *item) {
	Hash hashes[] = {
		Hash_fromPtr(item->ts),
		Hash_fromPtr(item->token),
		Hash_fromPtr(item->ctx),
		Type_hash(item->result.expect),
		Hash_fromInt(item->result.flags)
	};

	return Hash_combineArr(hashes, ARRSIZE(hashes));
}

HashMap_Key ParserCache_itemKey(ParserCache_Item *item) {
	return (HashMap_Key) { .match = &ParserCache_matchItem, .payload = item };
}

ParserResult *ParserCache_parseNode(ParserCache *this, TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParserCache_Item item = {
		.ctx = ctx,
		.ts = ts,
		.token = TokenStream_probe(ts),
		.result = *out,
		.log = Buffer_NULL,
		.token_after = NULL
	};

	bool existed;
	ParserCache_Item *cached = HashMap_ensure_probe(
		this->map, ParserCache_hashItem(&item), sizeof(ParserCache_Item),
		ParserCache_itemKey(&item), &existed, this->alc
	);

	if (existed) {
		return &cached->result;
	}

	*cached = item;

	ParserContext newctx = {
		.program_alc = this->alc,
		.tmp_alc = ctx->tmp_alc,
		.frame = ctx->frame,
		.logstream = StringOutStream_upcast(&this->log),
		.state = ctx->state
	};

	ParserResult *result = &cached->result;

	Parser_parseNode(ts, &newctx, result);

	cached->log = Buffer_copy(StringOutStream_view(&this->log), this->alc);
	StringOutStream_clear(&this->log);

	cached->token_after = TokenStream_probe(ts);

	return &cached->result;
}
