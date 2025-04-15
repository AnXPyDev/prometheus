#if defined(CCORE_MATH_FORCE_SSE2)

#define Matrix3_mulvfa Matrix3_mulvfa_sse2
#define Matrix3_mulv Matrix3_mulv_sse2
#define Matrix3_mul Matrix3_mul_sse2

#define Matrix4_mulvfa Matrix4_mulvfa_sse2
#define Matrix4_mulv Matrix4_mulv_sse2
#define Matrix4_mul Matrix4_mul_sse2

#define Quaternion_mul Quaternion_mul_sse2

void g_setupMathDispatch(void) {
	fprintf(stderr, "SSE2 math forced by compiler\n");
}

#elif defined(CCORE_MATH_FORCE_SSE42)

#define Matrix3_mulvfa Matrix3_mulvfa_sse42
#define Matrix3_mulv Matrix3_mulv_sse42
#define Matrix3_mul Matrix3_mul_sse42

#define Matrix4_mulvfa Matrix4_mulvfa_sse42
#define Matrix4_mulv Matrix4_mulv_sse42
#define Matrix4_mul Matrix4_mul_sse42

#define Quaternion_mul Quaternion_mul_sse42

void g_setupMathDispatch(void) {
	fprintf(stderr, "SSE4.2 math forced by compiler\n");
}

#elif defined(CCORE_MATH_FORCE_SCALAR)

#define Matrix3_mulvfa Matrix3_mulvfa_scalar
#define Matrix3_mulv Matrix3_mulv_scalar
#define Matrix3_mul Matrix3_mul_scalar

#define Matrix4_mulvfa Matrix4_mulvfa_scalar
#define Matrix4_mulv Matrix4_mulv_scalar
#define Matrix4_mul Matrix4_mul_scalar

#define Quaternion_mul Quaternion_mul_scalar

void g_setupMathDispatch(void) {
	fprintf(stderr, "Scalar math forced by compiler\n");
}

#else

// these are here to workaround a clangd bug

typedef Vector3F Vector3F_t;
typedef Vector3FA Vector3FA_t;
typedef Vector4F Vector4F_t;
typedef Vector4FA Vector4FA_t;
typedef QuaternionF QuaternionF_t;

Vector3FA_t (*Matrix3F_mulva)(const Matrix3F *, Vector3FA);
Vector3F_t (*Matrix3F_mulv)(const Matrix3F *, Vector3F);
void (*Matrix3F_mul)(const Matrix3F *, const Matrix3F *, Matrix3F *);

Vector4FA_t (*Matrix4F_mulva)(const Matrix4F *, Vector4FA);
Vector4F_t (*Matrix4F_mulv)(const Matrix4F *, Vector4F);
void (*Matrix4F_mult)(const Matrix4F *, const Matrix4F *, Matrix4F *);
void (*Matrix4F_mul)(const Matrix4F *, const Matrix4F *, Matrix4F *);

QuaternionF_t (*QuaternionF_mul)(QuaternionF, QuaternionF);

void g_setupMathDispatch(void) {
	if (g_CpuFeatures.sse42) {
		Matrix3F_mulva = &Matrix3F_mulva_sse42;
		Matrix3F_mulv = &Matrix3F_mulv_sse42;
		Matrix3F_mul = &Matrix3F_mul_sse42;
		
		Matrix4F_mulva = &Matrix4F_mulva_sse42;
		Matrix4F_mulv = &Matrix4F_mulv_sse42;
		Matrix4F_mult = &Matrix4F_mul_sse42;
		Matrix4F_mul = &Matrix4F_mul_sse42;
		
		QuaternionF_mul = &QuaternionF_mul_sse42;

		fprintf(stderr, "Dispatching math to SSE4.2\n");
	} else if (g_CpuFeatures.sse2) {
		Matrix3F_mulva = &Matrix3F_mulva_sse2;
		Matrix3F_mulv = &Matrix3F_mulv_sse2;
		Matrix3F_mul = &Matrix3F_mul_sse2;
		
		Matrix4F_mulva = &Matrix4F_mulva_sse2;
		Matrix4F_mulv = &Matrix4F_mulv_sse2;
		Matrix4F_mult = &Matrix4F_mul_sse2;
		Matrix4F_mul = &Matrix4F_mul_sse2;
		
		QuaternionF_mul = &QuaternionF_mul_sse2;

		fprintf(stderr, "Dispatching math to SSE2\n");
	} else {
		Matrix3F_mulva = &Matrix3F_mulva_scalar;
		Matrix3F_mulv = &Matrix3F_mulv_scalar;
		Matrix3F_mul = &Matrix3F_mul_scalar;
		
		Matrix4F_mulva = &Matrix4F_mulva_scalar;
		Matrix4F_mulv = &Matrix4F_mulv_scalar;
		Matrix4F_mult = &Matrix4F_mul_scalar;
		Matrix4F_mul = &Matrix4F_mul_scalar;
		
		QuaternionF_mul = &QuaternionF_mul_scalar;

		fprintf(stderr, "Dispatching math to Scalar\n");
	}
}

#endif
