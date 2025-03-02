void g_SimNode_setup_extension(void) {
	INode_GetNode.simext = ISimNode_GetNode;
	INode_SetNode.simext = ISimNode_SetNode;
	INode_CastNode.simext = ISimNode_CastNode;
	INode_SequenceNode.simext = ISimNode_SequenceNode;
	INode_ValueNode.simext = ISimNode_ValueNode;
	INode_ConditionNode.simext = ISimNode_ConditionNode;
	INode_FrameNode.simext = ISimNode_FrameNode;
	INode_BuiltinNode.simext = ISimNode_BuiltinNode;
}

SimValue SimNode_evaluate(Node this, SimContext *context) {
	return this.interface->simext.evaluate(this.object, context);
}
