// ValueNode
#define this ((ValueNode*)vthis)

int ValueNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return 0;
}

#undef this

const IParserNode IParserNode_ValueNode = {
	.eval_flags = &ValueNode_ParserNode_eval_flags
};

// GetNode
#define this ((GetNode*)vthis)

int GetNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	// if the current stack contains the memberlist owning the member we want to get
	// we need to rebuild the stack so the simulation can access it
	if	(ParserFrame_getMemberOwner(ctx->frame, this->member)) {
		// if there is no value available at parsetime for this member
		// it is impossible to simulate at parsetime
		if (!ParserFrame_getValue(ctx->frame, this->member))
			return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
		return PARSERNODE_EVAL_FLAG_STACK;
	}
	return 0;
}

#undef this

const IParserNode IParserNode_GetNode = {
	.eval_flags = &GetNode_ParserNode_eval_flags
};

// SetNode
#define this ((SetNode*)vthis)

int SetNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	// cannot modify parser frames upwards
	if (ParserFrame_getMemberOwner(ctx->frame, this->member))
		return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_SetNode = {
	.eval_flags = &SetNode_ParserNode_eval_flags
};

// SetPointerNode
#define this ((SetPointerNode*)vthis)

int SetPointerNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	int result = 0;
	if ((result |= ParserNode_eval_flags(this->pointer, ctx)) & PARSERNODE_EVAL_FLAG_IMPOSSIBLE)
		return result;

	result |= ParserNode_eval_flags(this->value, ctx);
	return result;
}

#undef this

const IParserNode IParserNode_SetPointerNode = {
	.eval_flags = &SetPointerNode_ParserNode_eval_flags
};

// GetPointerNode
#define this ((GetPointerNode*)vthis)

int GetPointerNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	return ParserNode_eval_flags(this->pointer, ctx);
}

#undef this

const IParserNode IParserNode_GetPointerNode = {
	.eval_flags = &GetPointerNode_ParserNode_eval_flags
};

// TakePointerNode
#define this ((TakePointerNode*)vthis)

int TakePointerNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	// TODO relax restrictions
	// cannot take pointer of parser frame values
	if (ParserFrame_getMemberOwner(ctx->frame, this->member))
		return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	return 0;
}

#undef this

const IParserNode IParserNode_TakePointerNode = {
	.eval_flags = &TakePointerNode_ParserNode_eval_flags
};

// CastNode
#define this ((CastNode*)vthis)

int CastNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_CastNode = {
	.eval_flags = &CastNode_ParserNode_eval_flags
};

// BuiltinNode
#define this ((BuiltinNode*)vthis)

int BuiltinNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	if (!(this->flags & BUILTIN_NODE_FLAG_PURE)) return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;

	int result = 0;

	Node *it = this->nodes;
	Node *end = it + this->size;

	for (; it < end; it++) {
		if ((result |= ParserNode_eval_flags(*it, ctx)) & PARSERNODE_EVAL_FLAG_IMPOSSIBLE)
			break;
	}

	return result;
}

#undef this

const IParserNode IParserNode_BuiltinNode = {
	.eval_flags = &BuiltinNode_ParserNode_eval_flags
};

// GetElementNode
#define this ((GetElementNode*)vthis)

int GetElementNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	// if the current stack contains the memberlist owning the member we want to get
	// we need to rebuild the stack so the simulation can access it
	if	(ParserFrame_getMemberOwner(ctx->frame, this->member)) {
		// if there is no value available at parsetime for this member
		// it is impossible to simulate at parsetime
		if (!ParserFrame_getValue(ctx->frame, this->member))
			return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
		return PARSERNODE_EVAL_FLAG_STACK;
	}
	return 0;
}

#undef this

const IParserNode IParserNode_GetElementNode = {
	.eval_flags = &GetElementNode_ParserNode_eval_flags
};

// SetElementNode
#define this ((SetElementNode*)vthis)

int SetElementNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	// cannot modify parser frames upwards
	if (ParserFrame_getMemberOwner(ctx->frame, this->member))
		return PARSERNODE_EVAL_FLAG_IMPOSSIBLE;
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_SetElementNode = {
	.eval_flags = &SetElementNode_ParserNode_eval_flags
};

// GetValueElementNode
#define this ((GetValueElementNode*)vthis)

int GetValueElementNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_GetValueElementNode = {
	.eval_flags = &GetValueElementNode_ParserNode_eval_flags
};

// GetPointerElementNode
#define this ((GetPointerElementNode*)vthis)

int GetPointerElementNode_ParserNode_eval_flags(void *vthis, ParserContext *ctx) {
	return ParserNode_eval_flags(this->value, ctx);
}

#undef this

const IParserNode IParserNode_GetPointerElementNode = {
	.eval_flags = &GetPointerElementNode_ParserNode_eval_flags
};
