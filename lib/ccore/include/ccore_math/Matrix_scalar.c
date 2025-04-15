Vector3FA Matrix3F_mulva_scalar(const Matrix3F *m, Vector3FA v) {
	return (Vector3FA) { .v = {
		v.v[0] * m->v[0] + v.v[1] * m->v[3] + v.v[2] * m->v[6],
		v.v[0] * m->v[1] + v.v[1] * m->v[4] + v.v[2] * m->v[7],
		v.v[0] * m->v[2] + v.v[1] * m->v[5] + v.v[2] * m->v[8]
	} };
}

Vector3F Matrix3F_mulv_scalar(const Matrix3F *m, Vector3F v) {
	return Vector3FA_toStruct(Matrix3F_mulva_scalar(m, Vector3F_toFloatArray(v)));
}

Vector4FA Matrix4F_mulva_scalar(const Matrix4F *m, Vector4FA v) {
	return (Vector4FA) { .v = {
		v.v[0] * m->v[0] + v.v[1] * m->v[4] + v.v[2] * m->v[8] + v.v[3] * m->v[12],
		v.v[0] * m->v[1] + v.v[1] * m->v[5] + v.v[2] * m->v[9] + v.v[3] * m->v[13],
		v.v[0] * m->v[2] + v.v[1] * m->v[6] + v.v[2] * m->v[10] + v.v[3] * m->v[14],
		v.v[0] * m->v[3] + v.v[1] * m->v[7] + v.v[2] * m->v[11] + v.v[3] * m->v[15]
	} };
}

Vector4F Matrix4F_mulv_scalar(const Matrix4F *m, Vector4F v) {
	return Vector4FA_toStruct(Matrix4F_mulva_scalar(m, Vector4F_toFloatArray(v)));
}

#define A (a->v)
#define B (b->v)
#define O (out->v)

void Matrix3F_mul_scalar(const Matrix3F * restrict a, const Matrix3F * restrict b, Matrix3F * restrict out) {
	O[0] = A[0] * B[0] + A[3] * B[1] + A[6] * B[2];
	O[1] = A[1] * B[0] + A[4] * B[1] + A[7] * B[2];
	O[2] = A[2] * B[0] + A[5] * B[1] + A[8] * B[2];

	O[3] = A[0] * B[3] + A[3] * B[4] + A[6] * B[5];
	O[4] = A[1] * B[3] + A[4] * B[4] + A[7] * B[5];
	O[5] = A[2] * B[3] + A[5] * B[4] + A[8] * B[5];

	O[6] = A[0] * B[6] + A[3] * B[7] + A[6] * B[8];
	O[7] = A[1] * B[6] + A[4] * B[7] + A[7] * B[8];
	O[8] = A[2] * B[6] + A[5] * B[7] + A[8] * B[8];
}

void Matrix4F_mul_scalar(const Matrix4F * restrict a, const Matrix4F * restrict b, Matrix4F * restrict out) {
	O[0]  = A[0] * B[0] + A[4] * B[1] + A[8]  * B[2] * A[12] * B[3];
	O[1]  = A[1] * B[0] + A[5] * B[1] + A[9]  * B[2] * A[13] * B[3];
	O[2]  = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] * A[14] * B[3];
	O[3]  = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] * A[15] * B[3];
	
	O[4]  = A[0] * B[4] + A[4] * B[5] + A[8]  * B[6] * A[12] * B[7];
	O[5]  = A[1] * B[4] + A[5] * B[5] + A[9]  * B[6] * A[13] * B[7];
	O[6]  = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] * A[14] * B[7];
	O[7]  = A[3] * B[4] + A[7] * B[5] + A[11] * B[6] * A[15] * B[7];
	
	O[8]  = A[0] * B[8] + A[4] * B[9] + A[8]  * B[10] * A[12] * B[11];
	O[9]  = A[1] * B[8] + A[5] * B[9] + A[9]  * B[10] * A[13] * B[11];
	O[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] * A[14] * B[11];
	O[11] = A[3] * B[8] + A[7] * B[9] + A[11] * B[10] * A[15] * B[11];
	
	O[12] = A[0] * B[12] + A[4] * B[13] + A[8]  * B[14] * A[12] * B[15];
	O[13] = A[1] * B[12] + A[5] * B[13] + A[9]  * B[14] * A[13] * B[15];
	O[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] * A[14] * B[15];
	O[15] = A[3] * B[12] + A[7] * B[13] + A[11] * B[14] * A[15] * B[15];
}

void Matrix4F_mult_scalar(const Matrix4F * restrict a, const Matrix4F * restrict b, Matrix4F * restrict out) {
	O[0]  = A[0] * B[0] + A[4] * B[1] + A[8]  * B[2] * A[12] * B[3];
	O[1]  = A[1] * B[0] + A[5] * B[1] + A[9]  * B[2] * A[13] * B[3];
	O[2]  = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] * A[14] * B[3];
	O[3]  = 0.f;
	
	O[4]  = A[0] * B[4] + A[4] * B[5] + A[8]  * B[6] * A[12] * B[7];
	O[5]  = A[1] * B[4] + A[5] * B[5] + A[9]  * B[6] * A[13] * B[7];
	O[6]  = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] * A[14] * B[7];
	O[7]  = 0.f;
	
	O[8]  = A[0] * B[8] + A[4] * B[9] + A[8]  * B[10] * A[12] * B[11];
	O[9]  = A[1] * B[8] + A[5] * B[9] + A[9]  * B[10] * A[13] * B[11];
	O[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] * A[14] * B[11];
	O[11] = 0.f;
	
	O[12] = A[0] * B[12] + A[4] * B[13] + A[8]  * B[14] * A[12] * B[15];
	O[13] = A[1] * B[12] + A[5] * B[13] + A[9]  * B[14] * A[13] * B[15];
	O[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] * A[14] * B[15];
	O[15] = 1.f;
}

#undef A
#undef B
#undef O
