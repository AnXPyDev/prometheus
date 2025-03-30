void g_SimNode_setup_extension(void) {
	INode_GetNode.simext = ISimNode_GetNode;
	INode_GetElementNode.simext = ISimNode_GetElementNode;
	INode_GetValueElementNode.simext = ISimNode_GetValueElementNode;
	INode_GetPointerElementNode.simext = ISimNode_GetPointerElementNode;
	INode_SetNode.simext = ISimNode_SetNode;
	INode_SetElementNode.simext = ISimNode_SetElementNode;
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
	INode_LoopNode.simext = ISimNode_LoopNode;
}

void SimNode_evaluate(Node this, SimContext *context, SimResult *result) {
	if (Node_isNull(this)) return;
	this.interface->simext.evaluate(this.object, context, result);
}
