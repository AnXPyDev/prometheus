#define this ((BufferView*)vthis)
void Printable_BufferView_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_write(os, *this);
}
#undef this

const IPrintable IPrintable_BufferView = {
	.print = &Printable_BufferView_print
};

#define bufrepr(b) ((Printable) { .interface = &IPrintable_BufferView, .object = (void*)((BufferView[]) {b}) })
#define strrepr(s) bufrepr(strview(s))
