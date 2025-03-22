#include "parsetree/include.h"

void Parser_parseNode(TokenStream *ts, ParserContext *ctx, ParserResult *out) {
	ParseTree_parseNode(ts, ctx, out);
}
