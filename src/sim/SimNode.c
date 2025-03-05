void g_SimNode_setup_extension(void) {
	INode_GetNode.simext = ISimNode_GetNode;
	INode_SetNode.simext = ISimNode_SetNode;
	INode_CastNode.simext = ISimNode_CastNode;
	INode_SequenceNode.simext = ISimNode_SequenceNode;
	INode_ValueNode.simext = ISimNode_ValueNode;
	INode_ConditionNode.simext = ISimNode_ConditionNode;
	INode_FrameNode.simext = ISimNode_FrameNode;
	INode_BuiltinNode.simext = ISimNode_BuiltinNode;
	INode_CallNode.simext = ISimNode_CallNode;
	INode_ControlNode.simext = ISimNode_ControlNode;
	INode_CatchNode.simext = ISimNode_CatchNode;
	INode_GetPointerNode.simext = ISimNode_GetPointerNode;
	INode_SetPointerNode.simext = ISimNode_SetPointerNode;
	INode_TakePointerNode.simext = ISimNode_TakePointerNode;
}

void SimNode_evaluate(Node this, SimContext *context, SimResult *result) {
	this.interface->simext.evaluate(this.object, context, result);
}
