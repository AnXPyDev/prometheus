typedef struct {
	StringView val;
} Expression_Test;


#define this ((Expression_Test*)vthis)

void Expression_Test_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "TEST({})", bufrepr(this->val));
}

#undef this

const IExpression IExpression_Test = {
	.print = &Expression_Test_print,
#ifdef EXPRESSION_EXT_SIM
	.ext_sim = ISimExpression_Test,
#endif
};

Expression Expression_Test_upcast(Expression_Test *this) {
	return (Expression) { .object = this, .interface = &IExpression_Test };
}
