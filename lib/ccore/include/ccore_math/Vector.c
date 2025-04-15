typedef struct {
	float x, y, z;
} Vector3F;

typedef struct {
	float x, y, z, w;
} Vector4F;

typedef struct {
	float v[3];
} Vector3FA;

typedef struct {
	float v[4];
} Vector4FA;

const Vector3F Vector3F_ZERO = { 0.f, 0.f, 0.f };
const Vector3F Vector3F_X = { 1.f, 0.f, 0.f };
const Vector3F Vector3F_Y = { 0.f, 1.f, 0.f };
const Vector3F Vector3F_Z = { 0.f, 0.f, 1.f };
const Vector3F Vector3F_NX = { -1.f, 0.f, 0.f };
const Vector3F Vector3F_NY = { 0.f, -1.f, 0.f };
const Vector3F Vector3F_NZ = { 0.f, 0.f, -1.f };

void Vector3F_fprint(FILE *file, Vector3F v) {
	fprintf(file, "Vector3F { x=%f, y=%f, z=%f }", v.x, v.y, v.z);
}

static inline Vector3F Vector3F_add(Vector3F a, Vector3F b) {
	return (Vector3F) { .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z };
}

static inline Vector3F Vector3F_sub(Vector3F a, Vector3F b) {
	return (Vector3F) { .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z };
}

static inline float Vector3F_length(Vector3F v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vector3F Vector3F_inverse(Vector3F v) {
	return (Vector3F) { .x = -v.x, .y = -v.y, .z = -v.z };
}

static inline Vector3F Vector3F_mul(Vector3F v, float m) {
	return (Vector3F) { .x = v.x * m, .y = v.y * m, .z = v.z * m };
}

static inline Vector3F Vector3F_normalize(Vector3F v) {
	const float m = Vector3F_length(v);
	return Vector3F_mul(v, 1.f / m);
}

static inline Vector3FA Vector3F_toFloatArray(Vector3F v) {
	return (Vector3FA) { .v = { v.x, v.y, v.z } };
}

static inline Vector4FA Vector4F_toFloatArray(Vector4F v) {
	return (Vector4FA) { .v = { v.x, v.y, v.z, v.w } };
}

static inline Vector3F Vector3FA_toStruct(Vector3FA vfa) {
	return (Vector3F) { .x = vfa.v[0], .y = vfa.v[1], .z = vfa.v[2] };
}

static inline Vector4F Vector4FA_toStruct(Vector4FA vfa) {
	return (Vector4F) { .x = vfa.v[0], .y = vfa.v[1], .z = vfa.v[2], .w = vfa.v[3] };
}

void Vector4F_fprint(FILE *file, Vector4F v) {
	fprintf(file, "Vector4F { x=%f, y=%f, z=%f, w=%f }", v.x, v.y, v.z, v.w);
}

static inline Vector4F Vector3F_toVector4(Vector3F v) {
	return (Vector4F) { .x = v.x, .y = v.y, .z = v.z, .w = 1.f };
}

static inline Vector3F Vector4F_toVector3(Vector4F v) {
	return (Vector3F) { .x = v.x, .y = v.y, .z = v.z };
}

#ifdef CCORE_PRINT_ENABLED

#define this ((Vector3F*)vthis)
void Printable_Vector3F_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Vector3F %{ x:{%f} y:{%f} z:{%f} %}",
		toPrintable(float, this->x),
		toPrintable(float, this->y),
		toPrintable(float, this->z)
	);
}
#undef this

const IPrintable IPrintable_Vector3F = {
	.print = &Printable_Vector3F_print
};

#define this ((Vector4F*)vthis)
void Printable_Vector4F_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "Vector4F %{ x:{%f} y:{%f} z:{%f} w:{%f}%}",
		toPrintable(float, this->x),
		toPrintable(float, this->y),
		toPrintable(float, this->z),
		toPrintable(float, this->w)
	);
}
#undef this

const IPrintable IPrintable_Vector4F = {
	.print = &Printable_Vector4F_print
};

#define Vector3F_repr(v) ((Printable) { .interface = &IPrintable_Vector3F, .object = (void*)((Vector3F[]) {v}) })
#define Vector4F_repr(v) ((Printable) { .interface = &IPrintable_Vector4F, .object = (void*)((Vector4F[]) {v}) })

#endif
