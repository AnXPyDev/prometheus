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

SimValue Sim_builtin_equals_(Array args, SimContext *context) {
	if (args.size != 2) {
		OutStream_puts(context->state->os_err, "Sim_builtin_equals_: wrong arg count\n");
		return SimValue_NULL;
	}

	SimValue *argv = args.data;

	if (SimValue_isNull(argv[0]) || SimValue_isNull(argv[1])) {
		return SimValue_NULL;
	}

	Size ts = Type_size(argv[0].type);

	if (ts != Type_size(argv[0].type)) {
		PrintFmt(context->state->os_err, "Sim_builtin_equals_ non matching types ({} != {})", Type_repr(argv[0].type), Type_repr(argv[1].type));
		return SimValue_NULL;
	}

	if (memcmp(argv[0].data, argv[1].data, ts) == 0) {
		return SimValue_INT_1;
	}

	return SimValue_INT_0;
}

const Sim_builtin_fn_t Sim_builtin_printArgs = &Sim_builtin_printArgs_;
const Sim_builtin_fn_t Sim_builtin_equals = &Sim_builtin_equals_;
