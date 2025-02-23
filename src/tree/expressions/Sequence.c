typedef struct {
	Array elements;
} Expression_Sequence;

void Expression_Sequence_create(Expression_Sequence *this, Array elements) {
	this->elements = elements;
}

#define this ((Expression_Sequence*)vthis)

void Expression_Sequence_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "SequenceExpression");
}

#undef this

const IExpression IExpression_Sequence = {
	.print = &Expression_Sequence_print,
#ifdef EXPRESSION_EXT_SIM
	.ext_sim = ISimExpression_Sequence,
#endif
};
