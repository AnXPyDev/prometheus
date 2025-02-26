typedef struct {
	Type argument;
	Type result;	
} FunctionType;

FunctionType FunctionType_new(Type A, Type R) {
	return (FunctionType) { .argument = A, .result = R };
}

#define this ((FunctionType*)vthis)

void FunctionType_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Function({} => {})", Type_repr(this->argument), Type_repr(this->result));
}

void FunctionType_info(void *vthis, TypeInfo *out_info) {
	TypeInfo ainfo, rinfo;
	Type_info(this->argument, &ainfo);
	Type_info(this->argument, &rinfo);

	*out_info = (TypeInfo) {
		.valid = ainfo.valid && rinfo.valid,
		.abstract = true,
		.size = 0
	};
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
};

Type FunctionType_upcast(FunctionType *this) {
	return (Type) { .interface = &IType_FunctionType, .object = this };
}
