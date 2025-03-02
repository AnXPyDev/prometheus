typedef SimValue SimValue_t;
typedef SimValue_t (*Sim_builtin_fn_t)(Array args, SimContext *context);

#define this ((BuiltinNode*)vthis)

SimValue BuiltinNode_SimNode_evaluate(void *vthis, SimContext *context) {
	SimValue *values = Allocator_malloc(context->temp_alc, sizeof(SimValue) * this->size);

	Node *end = this->nodes + this->size;
	SimValue *rp = values;

	for (Node *it = this->nodes; it < end; it++) {
		*(rp++) = SimNode_evaluate(*it, context);
	}

	Sim_builtin_fn_t builtin = *(Sim_builtin_fn_t*)this->builtin;

	return builtin((Array) { .data = values, .size = this->size }, context);
}

const ISimNode ISimNode_BuiltinNode = {
	.evaluate = &BuiltinNode_SimNode_evaluate
};

#undef this

SimValue Sim_builtin_printArgs_(Array args, SimContext *context) {
	SimValue *end = (SimValue*)args.data + args.size;
	SimValue *it;

	OutStream os = context->state->os_out;

	for (it = args.data; it < end - 1; it++) {
		SimValue_print(it, os, BufferView_NULL);
		OutStream_putc(os, ' ');
	}
	SimValue_print(it, os, BufferView_NULL);

	OutStream_putc(os, '\n');

	return SimValue_NULL;
}

const Sim_builtin_fn_t Sim_builtin_printArgs = &Sim_builtin_printArgs_;
