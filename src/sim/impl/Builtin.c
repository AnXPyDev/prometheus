typedef SimValue SimValue_t;
typedef void (*Sim_builtin_fn_t)(Array args, SimContext *context, SimResult *result);

#define this ((BuiltinNode*)vthis)

void BuiltinNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimValue *values = Allocator_malloc(context->temp_alc, sizeof(SimValue) * this->size);

	Node *end = this->nodes + this->size;
	SimValue *rp = values;

	for (Node *it = this->nodes; it < end; it++) {
		SimResult result = SimResult_NULL; SimNode_evaluate(*it, context, &result);
		if (result.control) {
			SimResult_forward(&result, out_result);
			goto interrupt;
		}
		*(rp++) = result.value;
	}

	Sim_builtin_fn_t builtin = *(Sim_builtin_fn_t*)this->builtin;

	builtin((Array) { .data = values, .size = this->size }, context, out_result);

	if (out_result->control) {
		out_result->control_origin = vthis;
		goto interrupt;
	}

	interrupt: 
	Allocator_free(context->temp_alc, values);

	return;
}

const ISimNode ISimNode_BuiltinNode = {
	.evaluate = &BuiltinNode_SimNode_evaluate
};

#undef this

void Sim_builtin_printArgs_(Array args, SimContext *context, SimResult *out_result) {
	SimValue *end = (SimValue*)args.data + args.size;
	SimValue *it;

	OutStream os = context->state->os_out;

	for (it = args.data; it < end - 1; it++) {
		SimValue_print(it, os, BufferView_NULL);
		OutStream_putc(os, ' ');
	}
	SimValue_print(it, os, BufferView_NULL);

	OutStream_putc(os, '\n');
}

void Sim_builtin_equals_(Array args, SimContext *context, SimResult *out_result) {
	if (args.size != 2) {
		SimResult_throwMessage("Sim_builtin_equals: wrong number of args", NULL, context, out_result);
		return;
	}

	SimValue *argv = args.data;

	if (SimValue_isNull(argv[0]) || SimValue_isNull(argv[1])) {
		SimResult_throwMessage("Sim_builtin_equals: arg is NULL", NULL, context, out_result);
		return;
	}

	Size ts = Type_size(argv[0].type);

	if (ts != Type_size(argv[0].type)) {
		SimResult_throwMessage("Sim_builtin_equals non matching types", NULL, context, out_result);
		return;
	}

	if (memcmp(argv[0].data, argv[1].data, ts) == 0) {
		out_result->value = SimValue_INT_1; return;
	}

	out_result->value = SimValue_INT_0; return;
}

void Sim_builtin_sum_ints_(Array args, SimContext *context, SimResult *out_result) {
	if (args.size == 0) {
		SimResult_throwMessage("Sim_builtin_sum: no args", NULL, context, out_result);
		return;
	}

	Type T_int = PrimitiveType_upcast(PRIMITIVE_TYPE_INT);

	int accum = 0;

	SimValue *it = args.data;
	SimValue *end = it + args.size;
	for (; it < end; it++) {
		if (SimValue_isNull(*it)) {
			SimResult_throwMessage("Sim_builtin_sum_ints: arg is NULL", NULL, context, out_result);
			return;
		}
		if (!Type_equal(T_int, it->type)) {
			SimResult_throwMessage("Sim_builtin_sum_ints: wrong arg type", NULL, context, out_result);
			return;
		}

		accum += *(int*)it->data;
	}

	out_result->value = SimValue_create(&accum, T_int, context->temp_alc);
	return;
}

const Sim_builtin_fn_t Sim_builtin_printArgs = &Sim_builtin_printArgs_;
const Sim_builtin_fn_t Sim_builtin_equals = &Sim_builtin_equals_;
const Sim_builtin_fn_t Sim_builtin_sum_ints = &Sim_builtin_sum_ints_;
