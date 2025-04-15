void Printable_NULL_print(void *vthis, OutStream os, StringView fmt) {
	OutStream_puts(os, "(null)");
}

const IPrintable IPrintable_NULL = {
	.print = &Printable_NULL_print
};

const Printable Printable_NULL = { .interface = &IPrintable_NULL, .object = NULL };
