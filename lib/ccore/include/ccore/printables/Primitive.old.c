void PrimitivePrintable_print_cstd(void *object, OutStream os, StringView fmt) {

}

void PrimitivePrintable_print(void *object, OutStream os, StringView fmt) {
	if (fmt.size == 0) {
		OutStream_putc(os, '?');
		return;
	}

	if (fmt.data[0] == '%' && fmt.size >= 2) {
		PrimitivePrintable_print_cstd(object, os, fmt);
	}

	if (fmt.size < 2 || fmt.data[0] != '%') {
		return;
	}

	char fbuf[256];
	char *fp = fbuf;
	
	char buf[256];
	char *bp = NULL;
	size_t s = 0;

	if (fmt.size > 255) {
		fp = malloc(fmt.size + 1);
	}

	memcpy(fp, fmt.data, fmt.size);
	fp[fmt.size] = (char)0;

	switch_type:;

	if (fmt.size > 2) {
		OutStream_putc(os, '?');
		goto quit;
	}

	switch (fmt.data[fmt.size - 1]) {
		case 'i': goto print_int;
		case 'd': goto print_int;
		case 'u': switch (fmt.data[fmt.size - 2]) {
		}
		case 'f': goto print_float;
		case 'p': goto print_pointer;
		default: {
			OutStream_putc(os, '?');
			goto quit;
		};
	}

	done_print:;
	if (bp) goto print_result;

	// alloc bp
	bp = buf;
	if (s > 255) {
		bp = malloc(s + 1);
	}
	s++;

	goto switch_type;

	print_result:;
	OutStream_write(os, (BufferView) { .data = bp, .size = s });

	quit:;
	if (fp && fp != fbuf) free(fp);
	if (bp != buf) free(bp);
	return;

	#define PRINT_CASE(T) { s = (size_t)snprintf(bp, s, fp, *(T*)object); goto done_print; }

	print_int: PRINT_CASE(int);
	print_unsigned_int: PRINT_CASE(unsigned int);
	print_float: PRINT_CASE(float);
	print_pointer: PRINT_CASE(void*);

	#undef PRINT_CASE

}

const IPrintable IPrimitivePrintable = {
	.print = &PrimitivePrintable_print
};

#define toPrintable(T, x) ((Printable) { .interface = &IPrimitivePrintable, .object = (void*)(&(T) {x})})

#define repr toPrintable
