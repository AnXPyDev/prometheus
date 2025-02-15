typedef struct {
	MemberList members;
} Expression_Frame;

void Expression_Frame_create(Expression_Frame *this, Allocator allocator) {
	MemberList_create(&this->members, allocator);
}

#define this ((Expression_Frame*)vthis)

void Expression_Frame_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "FrameExpression");
}

#undef this

const IExpression IExpression_Frame = {
	.print = &Expression_Frame_print,
#ifdef EXPRESSION_EXT_SIM
	.ext_sim = ISimExpression_Frame,
#endif
};
