typedef size_t Size;

#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))

#define TYPE_DECL(type, name) typedef type name name

#define ARRSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define TEMP_LOCAL(T, expr) (((struct { T val; }) {.val = (expr)}).val)

#define foreach(T, it, begin, end) for (T *it = (T*)(begin), * const _end = (T*)(end); it < _end; it++)

Size memalign(Size offset, Size alignment) {
	Size d = offset % alignment;
	return offset + (alignment - d) * !!d;
}
