typedef struct {
	void (*print)(void*, OutStream os, StringView fmt);
} IPrintable;

typedef struct {
	void *object;
	const IPrintable *interface;
} Printable;

void Printable_print(Printable this, OutStream os, StringView fmt) {
	this.interface->print(this.object, os, fmt);
}

static inline void PrintFmt_helper(OutStream os, const char **fc, Printable arg) {
	(*fc)++;
	StringView fmt = {
		.data = *fc,
		.size = 0
	};

	while (**fc != '}') {
		(*fc)++;
		fmt.size++;
	}

	Printable_print(arg, os, fmt);
}

void PrintFmt_(OutStream os, StringView fmt, Printable *args) {
	Printable *arg = args;
	const char *fc = fmt.data;
	const char *end = fmt.data + fmt.size;

	bool escape = false;

	while (fc < end) {
		if (!escape) {
			if (*fc == '{') {
				PrintFmt_helper(os, &fc, *arg);
				arg++;
				goto next_char;
			} else if (*fc == '%') {
				escape = true;
				goto next_char;
			}
		}

		escape = false;
		OutStream_putc(os, *fc);

		next_char:;
		fc++;
	}
}

#define PrintFmt(os, fmt, ...) PrintFmt_(os, strview(fmt), (Printable[]) {__VA_ARGS__})

#define CCORE_PRINT_ENABLED
