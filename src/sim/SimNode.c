SimValue SimNode_evaluate(Node this, SimContext *context) {
	return this.interface->simext.evaluate(this.object, context);
}
