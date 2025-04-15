typedef Vector4F QuaternionF;

const QuaternionF QuaternionF_IDENTITY = {
	.w = 1.f, .x = 0.f, .y = 0.f, .z = 0.f
};

static inline QuaternionF QuaternionF_add(QuaternionF a, QuaternionF b) {
	return (QuaternionF) {
		.w = a.w + b.w,
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
}

static inline QuaternionF QuaternionF_sub(QuaternionF a, QuaternionF b) {
	return (QuaternionF) {
		.w = a.w - b.w,
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
}

static inline QuaternionF QuaternionF_mul_scalar(QuaternionF a, QuaternionF b) {
	return (QuaternionF) {
		.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
		.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
		.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
	};
}

static inline float QuaternionF_norm(QuaternionF q) {
	return sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

static inline QuaternionF QuaternionF_normalize(QuaternionF q) {
	const float m = QuaternionF_norm(q);
	return (QuaternionF) {
		.w = q.w / m,
		.x = q.x / m,
		.y = q.y / m,
		.z = q.z / m
	};
}

static inline QuaternionF QuaternionF_uinvert(QuaternionF q) {
	return (QuaternionF) { .w = q.w, .x = -q.x, .y = -q.y, .z = -q.z};
}

static inline QuaternionF QuaternionF_invert(QuaternionF q) {
	const float m = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
	return (QuaternionF) { .w = q.w / m, .x = (-q.x) / m, .y = (-q.y) / m, .z = (-q.z) / m};
}

void QuaternionF_print(FILE *file, QuaternionF q) {
	fprintf(file, "QuaternionF { x: %f, y: %f, z: %f, w: %f }", q.w, q.x, q.y, q.z);
}

static inline QuaternionF QuaternionF_fromNAxis(Vector3F axis, float angle) {
	const float half_angle = angle / 2.f;
	const float sine = sinf(half_angle);
	return (QuaternionF) {
		.w = cosf(half_angle),
		.x = axis.x * sine,
		.y = axis.y * sine,
		.z = axis.z * sine
	};
}

static inline QuaternionF QuaternionF_fromAxis(Vector3F axis, float angle) {
	return QuaternionF_fromNAxis(Vector3F_normalize(axis), angle);
}

static inline void QuaternionF_toRealIJK(QuaternionF q, float *i, float *j, float *k) {
	const float f_w2 = q.w * q.w;
	const float f_x2 = q.x * q.x;
	const float f_y2 = q.y * q.y;
	const float f_z2 = q.z * q.z;

	const float f_2wx = 2.f * q.w * q.x;
	const float f_2wy = 2.f * q.w * q.y;
	const float f_2wz = 2.f * q.w * q.z;
	const float f_2xy = 2.f * q.x * q.y;
	const float f_2xz = 2.f * q.x * q.z;
	const float f_2yz = 2.f * q.y * q.z;

	i[0] = f_w2 + f_x2 - f_y2 - f_z2;
	i[1] = f_2xy - f_2wz;
	i[2] = f_2xz + f_2wy;

	j[0] = f_2wz + f_2xy;
	j[1] = f_w2 - f_x2 + f_y2 - f_z2;
	j[2] = f_2yz - f_2wx;

	k[0] = f_2xz - f_2wy;
	k[1] = f_2wx + f_2yz;
	k[2] = f_w2 - f_x2 - f_y2 + f_z2;
}

static inline void QuaternionF_toRealIJK_TP(QuaternionF q, float *i, float *j, float *k) {
	const float f_w2 = q.w * q.w;
	const float f_x2 = q.x * q.x;
	const float f_y2 = q.y * q.y;
	const float f_z2 = q.z * q.z;

	const float f_2wx = 2.f * q.w * q.x;
	const float f_2wy = 2.f * q.w * q.y;
	const float f_2wz = 2.f * q.w * q.z;
	const float f_2xy = 2.f * q.x * q.y;
	const float f_2xz = 2.f * q.x * q.z;
	const float f_2yz = 2.f * q.y * q.z;

	i[0] = f_w2 + f_x2 - f_y2 - f_z2;
	j[0] = f_2xy - f_2wz;
	k[0] = f_2xz + f_2wy;

	i[1] = f_2wz + f_2xy;
	j[1] = f_w2 - f_x2 + f_y2 - f_z2;
	k[1] = f_2yz - f_2wx;

	i[2] = f_2xz - f_2wy;
	j[2] = f_2wx + f_2yz;
	k[2] = f_w2 - f_x2 - f_y2 + f_z2;
}

static inline void QuaternionF_toTransformMatrix4F(QuaternionF q, Matrix4F *out_m) {
	QuaternionF_toRealIJK(q, &out_m->v[0], &out_m->v[4], &out_m->v[8]);
	out_m->v[3] = 0.f;
	out_m->v[7] = 0.f;
	out_m->v[11] = 0.f;
}

static inline void QuaternionF_toTransformMatrix4F_TP(QuaternionF q, Matrix4F *out_m) {
	QuaternionF_toRealIJK_TP(q, &out_m->v[0], &out_m->v[4], &out_m->v[8]);
	out_m->v[3] = 0.f;
	out_m->v[7] = 0.f;
	out_m->v[11] = 0.f;
}

static inline void QuaternionF_toRotationMatrix3F(QuaternionF q, Matrix3F *out_m) {
	QuaternionF_toRealIJK(q, &out_m->v[0], &out_m->v[3], &out_m->v[6]);
}

static inline void QuaternionF_toRotationMatrix3F_TP(QuaternionF q, Matrix3F *out_m) {
	QuaternionF_toRealIJK_TP(q, &out_m->v[0], &out_m->v[3], &out_m->v[6]);
}

#define QuaternionF_mul_sse42 QuaternionF_mul_scalar
#define QuaternionF_mul_sse2 QuaternionF_mul_scalar

#ifdef CCORE_PRINT_ENABLED

#define this ((QuaternionF*)vthis)
void Printable_QuaternionF_print(void *vthis, OutStream os, StringView fmt) {
	PrintFmt(os, "QuaternionF %{ x:{%f} y:{%f} z:{%f} w:{%f}%}",
		toPrintable(float, this->x),
		toPrintable(float, this->y),
		toPrintable(float, this->z),
		toPrintable(float, this->w)
	);
}
#undef this

const IPrintable IPrintable_QuaternionF = {
	.print = &Printable_QuaternionF_print
};

#define QuaternionF_repr(q) ((Printable) { .interface = &IPrintable_Vector4F, .object = (void*)((QuaternionF[]) {q}) })

#endif

