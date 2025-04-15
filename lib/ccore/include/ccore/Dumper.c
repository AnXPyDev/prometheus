typedef enum {
	DUMPER_FLAG_DESCRIBE = 1 << 0,
	DUMPER_FLAG_NOSIZE = 1 << 1, 
} EDumper_Flag;

typedef struct {
	int flags;
	BufferView buffer;
} Dumper;

Dumper Dumper_new(BufferView buffer, int flags) {
	return (Dumper) { .flags = flags, .buffer = buffer };
}

#define this ((Dumper*)vthis)

void Dumper_print_nosize(void *vthis, OutStream os, StringView fmt) {

}

void Dumper_print(void *vthis, OutStream os, StringView fmt) {
	if (!(this->flags & DUMPER_FLAG_NOSIZE)) {
		PrintFmt(os, "[{}]", repr(Size, this->buffer.size));
	}

	if (this->buffer.size == 0) {
		OutStream_puts(os, "(null)");
		return;
	}

	char hexdump[257];

	if ((this->flags & DUMPER_FLAG_DESCRIBE) || this->buffer.size > (sizeof(hexdump) / 2)) {
		PrintFmt(os, "@{%p}", repr(ptr_t, this->buffer.data));
		return;
	}

	char *hxend = hexdump + sizeof(hexdump);
	char *hxp = hexdump;
	const char *end = this->buffer.data + this->buffer.size;
	for (const char *it = this->buffer.data; it < end; it++) {
		Size s = (Size)(hxend - hxp);
		int n = snprintf(hxp, (size_t)(hxend - hxp), "%02X", (unsigned char)(*it));

		if (n < 0 || (Size)n >= s) break;

		#ifdef __ORDER_LITTLE_ENDIAN__
		if (n == 2) {
			char c1 = hxp[0];
			hxp[0] = hxp[1];
			hxp[1] = c1;
		}
		#endif
		
		hxp += n;
		
		if (hxp + 1 >= hxend) break;	
	}

	OutStream_putc(os, '=');
	OutStream_puts(os, hexdump);

	#undef SIZE

}

#undef this

const IPrintable IPrintable_Dumper = {
	.print = &Dumper_print,
};

Printable Dumper_repr(Dumper *this) {
	return (Printable) { .interface = &IPrintable_Dumper, .object = this };
}
