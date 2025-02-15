typedef struct {
	void (*print)(void*, OutStream, StringView);
#ifdef EXPRESSION_EXT_SIM
	const ISimExpression ext_sim;
#endif
} IExpression;

typedef struct {
	void *object;
	const IExpression *interface;
} Expression;

#define this ((Expression*)vthis)
void Printable_Expression_print(void *vthis, OutStream os, StringView fmt) {
	this->interface->print(this->object, os, fmt);
}
#undef this

const IPrintable IPrintable_Expression = {
	.print = &Printable_Expression_print
};

Printable Expression_repr(Expression *this) {
	return (Printable) { .object = (void*)this, .interface = &IPrintable_Expression };
}

const Expression Expression_NULL = {
	.object = NULL, .interface = NULL
};

bool Expression_isNull(Expression this) {
	return this.object == NULL;
}
