// column major matrices

typedef struct {
	float v[16];
} Matrix4F;

typedef struct {
	float v[9];
} Matrix3F;

void Matrix3F_print(FILE *file, Matrix3F *m) {
	fprintf(file, "Matrix3F [\n");
	for (int i = 0; i < 3; i++) {
		fprintf(file, "  [ %5.2f %5.2f %5.2f ]\n", m->v[i], m->v[i+3], m->v[i+6]);
	}
	fprintf(file, "]\n");
}

void Matrix4F_print(FILE *file, Matrix4F *m) {
	fprintf(file, "Matrix4 [\n");
	for (int i = 0; i < 4; i++) {
		fprintf(file, "  [ %5.2f %5.2f %5.2f %5.2f ]\n", m->v[i], m->v[i+4], m->v[i+8], m->v[i+12]);
	}
	fprintf(file, "]\n");
}

void Matrix3F_transpose(const Matrix3F *m, Matrix3F *out) {
	out->v[0] = m->v[0];
	out->v[4] = m->v[4];
	out->v[8] = m->v[8];
	
	out->v[1] = m->v[3];
	out->v[2] = m->v[6];
	out->v[3] = m->v[1];

	out->v[5] = m->v[7];
	out->v[6] = m->v[2];
	out->v[7] = m->v[5];
}

void Matrix3F_transposeInPlace(Matrix3F *m) {
	const float m1 = m->v[1];
	const float m2 = m->v[2];
	const float m5 = m->v[5];

	m->v[1] = m->v[3];
	m->v[2] = m->v[6];
	m->v[3] = m1;

	m->v[5] = m->v[7];
	m->v[6] = m2;
	m->v[7] = m5;
}

static inline void Matrix4F_setCol(Matrix4F *m, int ix, float *vals) {
	const int ci = ix * 4;
	m->v[ci] = vals[0];
	m->v[ci + 1] = vals[1];
	m->v[ci + 2] = vals[2];
	m->v[ci + 3] = vals[3];
}

static inline void Matrix4F_setTranslate(Matrix4F *m, Vector3F v) {
	float vals[4] = { v.x, v.y, v.z, 1.f };
	Matrix4F_setCol(m, 3, vals);
}

static inline void Matrix4F_setTransform(Matrix4F *m, Matrix3F *t) {
	m->v[0] = t->v[0];
	m->v[1] = t->v[1];
	m->v[2] = t->v[2];
	m->v[3] = 0.f;
	
	m->v[4] = t->v[3];
	m->v[5] = t->v[4];
	m->v[6] = t->v[5];
	m->v[7] = 0.f;
	
	m->v[8] = t->v[6];
	m->v[9] = t->v[7];
	m->v[10] = t->v[8];
	m->v[11] = 0.f;
}

static inline void Matrix3F_setIdentity(Matrix3F *m) {
	m->v[0] = 1.f; m->v[4] = 1.f; m->v[8] = 1.f;
}

static inline void Matrix4F_setIdentity(Matrix4F *m) {
	m->v[0] = 1.f; m->v[5] = 1.f; m->v[10] = 1.f; m->v[15] = 1.f;
}

#include "Matrix_scalar.c"
#include "Matrix_sse42.c"
#include "Matrix_sse2.c"
