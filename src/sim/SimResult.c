void SimResult_throwMessage(const char *message, void *origin, SimContext *context, SimResult *out_result) {
	out_result->control = SIM_CONTROL_SIGNAL_THROW;
	out_result->control_origin = origin;
	out_result->value = SimValue_create(
		&message, PrimitiveType_upcast(PRIMITIVE_TYPE_MESSAGE), context->temp_alc
	);
}

void SimResult_copy(SimResult *from, SimResult *to, SimContext *context) {
	to->control = from->control;
	to->control_origin = from->control_origin;
	to->control_target = from->control_target;
	to->value = SimValue_copy(from->value, context->temp_alc);
}
