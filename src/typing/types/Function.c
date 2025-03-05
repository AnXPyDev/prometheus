typedef struct {
	Type argument;
	Type result;	
} FunctionType;

FunctionType *FunctionType_create(Type A, Type R, Allocator alc) {
	FunctionType *this = Allocator_malloc(alc, sizeof(FunctionType));
	this->argument = Type_copy(A, alc);
	this->result = Type_copy(R, alc);
	return this;
}

Type FunctionType_upcast(FunctionType *this);

#define this ((FunctionType*)vthis)

void FunctionType_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Function<{} => {}>", Type_repr(this->argument), Type_repr(this->result));
}

void FunctionType_info(void *vthis, TypeInfo *out_info) {
	TypeInfo ainfo, rinfo;
	Type_info(this->argument, &ainfo);
	Type_info(this->argument, &rinfo);

	*out_info = (TypeInfo) {
		.valid = ainfo.valid && rinfo.valid,
		.abstract = true,
		.size = PRIMITIVE_TYPE_ABSTRACT_SIZE
	};
}

Type FunctionType_copy(void *vthis, Allocator alc) {
	return FunctionType_upcast(FunctionType_create(this->argument, this->result, alc));
}

void FunctionType_destroy(void *vthis, Allocator alc) {
	Type_destroy(this->argument, alc);
	Type_destroy(this->result, alc);
	Allocator_free(alc, vthis);
}

#undef this

const IPrintable IPrintable_FunctionType = {
	.print = &FunctionType_print
};

Printable FunctionType_repr(void *vthis) {
	return (Printable) { .interface = &IPrintable_FunctionType, .object = vthis };
}

const IType IType_FunctionType = {
	.repr_ = &FunctionType_repr,
	.info = &FunctionType_info,
	.copy = &FunctionType_copy,
	.destroy = &FunctionType_destroy
};

Type FunctionType_upcast(FunctionType *this) {
	return (Type) { .interface = &IType_FunctionType, .object = this };
}

bool Type_isFunctionType(Type this) {
	return this.interface == &IType_FunctionType;
}
