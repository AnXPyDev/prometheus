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

