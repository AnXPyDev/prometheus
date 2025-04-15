#include "config.h"

#include "Vector.c"
#include "Matrix.c"
#include "Quaternion.c"

#ifndef CCORE_MATH_DISABLE_CPUFEATURES
#include "cpufeatures.c"
#else
#define CCORE_MATH_FORCE_SCALAR
#endif

#include "dispatch.c"
