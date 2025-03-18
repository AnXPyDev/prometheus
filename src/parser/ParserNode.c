void g_ParserNode_setup_extension(void) {
	INode_GetNode.pext = IParserNode_GetNode;
	INode_SetNode.pext = IParserNode_SetNode;
	INode_CastNode.pext = IParserNode_CastNode;
	INode_SequenceNode.pext = IParserNode_SequenceNode;
	INode_ValueNode.pext = IParserNode_ValueNode;
	INode_ConditionNode.pext = IParserNode_ConditionNode;
	INode_FrameNode.pext = IParserNode_FrameNode;
	INode_BuiltinNode.pext = IParserNode_BuiltinNode;
	INode_CallNode.pext = IParserNode_CallNode;
	INode_ControlNode.pext = IParserNode_ControlNode;
	INode_CatchNode.pext = IParserNode_CatchNode;
	INode_GetPointerNode.pext = IParserNode_GetPointerNode;
	INode_SetPointerNode.pext = IParserNode_SetPointerNode;
	INode_TakePointerNode.pext = IParserNode_TakePointerNode;
	INode_LoopNode.pext = IParserNode_LoopNode;
}

int ParserNode_eval_flags(Node this, ParserContext *ctx) {
	if (Node_isNull(this)) return 0;
	return this.interface->pext.eval_flags(this.object, ctx);
}

SimStackFrame *ParserFrame_toSimFrame(ParserFrame *this, SimState *simstate) {
	SimStackFrame *parent = NULL;
	if (this->parent) {
		parent = ParserFrame_toSimFrame(this->parent, simstate);
	}

	SimStackFrame *simframe = SimStackFrame_create(
		parent,
		this->memberlist,
		SimCache_getMemberList(&simstate->cache, this->memberlist),
		simstate->alc, simstate->alc
	);

	{
		Array members = MemberList_members(this->memberlist);
		Member **it = members.data;
		Member **end = it + members.size;
		for (; it < end; it++) {
			Member *member = *it;
			void *val = ParserFrame_getValue(this, member);
			if (!val) continue;

			memcpy(
				SimStackFrame_getValue(simframe, member),
				val,
				simframe->mlinfo->info[member->index].type_size
			);
		}
	}

	return simframe;
}

void ParserNode_evaluate(Node this, int flags, ParserContext *ctx, ParserResult *out) {
	if (flags & PARSERNODE_EVAL_FLAG_STACK) {
		Parser_throw(ctx, NULL, PARSER_RESULT_PANIC, "Cannot evaluate with stack", out);
		return;
	}

	SimState simstate = {
		.os_err = OutStream_NULL,
		.os_out = OutStream_NULL,
		.alc = ctx->tmp_alc,
		.temp_alc = ctx->tmp_alc,
		.root_frame = NULL
	};

	SimState_init(&simstate);

	SimContext simctx = {
		.state = &simstate,
		.frame = NULL,
		.temp_alc = ctx->tmp_alc
	};

	SimResult result = SimResult_NULL;
	SimNode_evaluate(this, &simctx, &result);
	if (result.control) {
		Parser_throw(ctx, NULL, PARSER_RESULT_PANIC, "Eval interrupt", out);
		return;
	}

	if (SimValue_isNull(result.value)) {
		Parser_throw(ctx, NULL, PARSER_RESULT_PANIC, "Eval null", out);
		return;
	}

	out->node = ValueNode_create(result.value.type, result.value.data, ctx->state->program_alc);
}
