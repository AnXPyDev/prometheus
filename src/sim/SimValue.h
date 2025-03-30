typedef struct {
	Type type;
	const void *data;
} SimValue;

const SimValue SimValue_NULL = { .type = Type_NULL, .data = NULL };

bool SimValue_isNull(SimValue this) {
	return this.data == NULL || Type_isNull(this.type);
}

SimValue SimValue_create(const void *data, Type T, Allocator alc) {
	Type type = Type_copy(T, alc);
	Size ts = Type_size(type);

	if (ts == 0) {
		return SimValue_NULL;	
	}

	void *new_data = Allocator_malloc(alc, ts);
	memcpy(new_data, data, ts);

	return (SimValue) { .type = type, .data = new_data };
}

SimValue SimValue_create_nocopy(const void *data, Type T) {
	return (SimValue) {
		.type = T, .data = data
	};
}

SimValue SimValue_copy(SimValue this, Allocator alc) {
	if (SimValue_isNull(this)) return SimValue_NULL;
	return SimValue_create(this.data, this.type, alc);
}

#define this ((SimValue*)vthis)

void SimValue_print(void *vthis, OutStream os, StringView fmt) {
	Size ts = Type_size(this->type);
	Dumper dumper = Dumper_new((BufferView) { .size = ts, .data = this->data }, 0);
	PrintFmt(os, "SimValue({}; {})", Type_repr(this->type), Dumper_repr(&dumper));
}

#undef this

const IPrintable IPrintable_SimValue = {
	.print = &SimValue_print
};

Printable SimValue_repr(SimValue *this) {
	if (SimValue_isNull(*this)) return Printable_NULL;
	return (Printable) { .interface = &IPrintable_SimValue, .object = this };
}

SimValue SimValue_INT_1;
SimValue SimValue_INT_0;
