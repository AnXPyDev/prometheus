typedef SimValue SimValue_t;
typedef void (*Sim_builtin_fn_t)(Array args, SimContext *context, SimResult *result);

#define this ((BuiltinNode*)vthis)

void BuiltinNode_SimNode_evaluate(void *vthis, SimContext *context, SimResult *out_result) {
	SimValue *values = Allocator_malloc(context->temp_alc, sizeof(SimValue) * this->size);

	Node *end = this->nodes + this->size;
	SimValue *rp = values;
	SimResult result;
	for (Node *it = this->nodes; it < end; it++) {
		result = SimResult_NULL; SimNode_evaluate(*it, context, &result);
		if (result.control) {
			switch (result.control) {
				case SIM_CONTROL_SIGNAL_EMIT:
					goto handle_emit;
				default:;
			}

			if (0) handle_emit: {
				if (result.control_target == vthis) {
					out_result->value = result.value;
					goto quit;
				}
			}

			goto interrupt;
		}
		*(rp++) = result.value;
	}
	
	if (0) interrupt: {
		SimResult_forward(&result, out_result);
		goto quit;
	}

	Sim_builtin_fn_t builtin = *(Sim_builtin_fn_t*)this->builtin;

	result = SimResult_NULL;

	builtin((Array) { .data = values, .size = this->size }, context, &result);

	if (result.control) {
		switch (result.control) {
			case SIM_CONTROL_SIGNAL_EMIT:
				goto handle_emit_2;
			default:;
		}

		if (0) handle_emit_2: {
			if (result.control_target == vthis) {
				goto return_result;
			}
		}
		SimResult_forward(&result, out_result);
	}

	return_result:;
	out_result->value = result.value;

	quit:;
	Allocator_free(context->temp_alc, values);
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

void Sim_builtin_neg_int_(Array args, SimContext *context, SimResult *out_result) {
	if (args.size != 1) {
		SimResult_throwMessage("Sim_builtin_neg: only one arg allowed", NULL, context, out_result);
		return;
	}

	Type T_int = PrimitiveType_upcast(PRIMITIVE_TYPE_INT);

	SimValue *val = args.data;

	int res = 0 - *(int*)val->data;

	out_result->value = SimValue_create(&res, T_int, context->temp_alc);
	return;
}

const Sim_builtin_fn_t Sim_builtin_printArgs = &Sim_builtin_printArgs_;
const Sim_builtin_fn_t Sim_builtin_equals = &Sim_builtin_equals_;
const Sim_builtin_fn_t Sim_builtin_sum_ints = &Sim_builtin_sum_ints_;
const Sim_builtin_fn_t Sim_builtin_neg_int = &Sim_builtin_neg_int_;
