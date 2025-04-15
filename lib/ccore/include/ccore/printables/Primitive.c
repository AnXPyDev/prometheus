typedef void *ptr_t;

#define GENERATE_PRINTF_FORWARD(T, FMT) \
void PrimitivePrintable_print_##T(void *object, OutStream os, StringView fmt) { \
	char fbuf[64] = "%"; memcpy(fbuf + 1, fmt.data, fmt.size); \
	strcpy(fbuf + (fmt.size + 1), FMT); \
	char buf[256]; \
	sprintf(buf, fbuf, *(T*)object); \
	OutStream_puts(os, buf); \
}

GENERATE_PRINTF_FORWARD(int, "d")
GENERATE_PRINTF_FORWARD(Size, "zu")
GENERATE_PRINTF_FORWARD(size_t, "zu")
GENERATE_PRINTF_FORWARD(ptr_t, "p")
GENERATE_PRINTF_FORWARD(ptrdiff_t, "td")

#undef GENERATE_PRINTF_FORWARD

#define repr(T, V) (Printable) { \
	.interface = &(IPrintable) { \
		.print = PrimitivePrintable_print_##T \
	}, \
	.object = &(T) { (T)V } \
}
