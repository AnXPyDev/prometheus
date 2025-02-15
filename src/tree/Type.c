typedef enum {
	PRIMITIVE_TYPE_VOID = 0,
	PRIMITIVE_TYPE_INT,
	PRIMITIVE_TYPE_PTR,
	PRIMITIVE_TYPE__END
} EPrimitiveType;

const char *EPrimitiveType_repr[PRIMITIVE_TYPE__END] = {
	[PRIMITIVE_TYPE_VOID] = "PRIMITIVE_TYPE_VOID",
	[PRIMITIVE_TYPE_INT] = "PRIMITIVE_TYPE_INT",
	[PRIMITIVE_TYPE_PTR] = "PRIMITIVE_TYPE_PTR",
};

typedef struct {
	EPrimitiveType val;
} Type;

const Type g_Type_Primitive[PRIMITIVE_TYPE__END] = {
	[PRIMITIVE_TYPE_VOID] = { .val = PRIMITIVE_TYPE_VOID },
	[PRIMITIVE_TYPE_INT] = { .val = PRIMITIVE_TYPE_INT },
	[PRIMITIVE_TYPE_PTR] = { .val = PRIMITIVE_TYPE_PTR }
};

typedef struct {
	Size size;
} TypeInfo;

const TypeInfo g_Type_Primitive_info[PRIMITIVE_TYPE__END] = {
	[PRIMITIVE_TYPE_VOID] = { .size = 0 },
	[PRIMITIVE_TYPE_INT] = { .size = sizeof(int) },
	[PRIMITIVE_TYPE_PTR] = { .size = sizeof(void*) },

};

TypeInfo Type_info(Type this) {
	return g_Type_Primitive_info[this.val];
}
