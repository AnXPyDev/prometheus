typedef struct {
	Type argument;
	Type result;	
} FunctionType;

Type FunctionType_upcast(FunctionType *this);
Type ConstFunctionType_upcast(FunctionType *this);

Type FunctionType_create(Type A, Type R, Allocator alc) {
	FunctionType *this = Allocator_malloc(alc, sizeof(FunctionType));
	this->argument = Type_copy(A, alc);
	this->result = Type_copy(R, alc);
	return FunctionType_upcast(this);
}

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
		.size = sizeof(FunctionValue)
	};
}

Type FunctionType_copy(void *vthis, Allocator alc) {
	return FunctionType_create(this->argument, this->result, alc);
}

Type FunctionType_constcast(void *vthis) {
	return ConstFunctionType_upcast(this);
}

Type FunctionType_recast(void *vthis) {
	return FunctionType_upcast(this);
}

Type ConstFunctionType_copy(void *vthis, Allocator alc) {
	return ConstFunctionType_upcast(this);
}

void FunctionType_destroy(void *vthis, Allocator alc) {
	Type_destroy(this->argument, alc);
	Type_destroy(this->result, alc);
	Allocator_free(alc, vthis);
}

void ConstFunctionType_destroy(void *vthis, Allocator alc) {}

bool FunctionType_equal(void *vthis, void *vother) {
	FunctionType *other = vother;
	return Type_equal(this->argument, other->argument) && Type_equal(this->result, other->result);
}

Hash FunctionType_hash(void *vthis) {
	return Hash_combine(
		Hash_fromIntPtr((intptr_t)&FunctionType_hash),
		Hash_combine(
			Type_hash(this->argument),
			Type_hash(this->result)
		)
	);
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
	.destroy = &FunctionType_destroy,
	.equal = &FunctionType_equal,
	.constcast = &FunctionType_constcast,
	.hash = &FunctionType_hash
};

const IType IType_ConstFunctionType = {
	.repr_ = &FunctionType_repr,
	.info = &FunctionType_info,
	.copy = &ConstFunctionType_copy,
	.destroy = &ConstFunctionType_destroy,
	.equal = &FunctionType_equal,
	.constcast = &FunctionType_constcast,
	.recast = &FunctionType_recast,
	.hash = &FunctionType_hash
};

Type FunctionType_upcast(FunctionType *this) {
	return (Type) { .interface = &IType_FunctionType, .object = this };
}

Type ConstFunctionType_upcast(FunctionType *this) {
	return (Type) { .interface = &IType_FunctionType, .object = this };
}

bool Type_isFunctionType(Type this) {
	return this.interface == &IType_FunctionType;
}
