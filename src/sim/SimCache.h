typedef struct {
	Allocator alc;
	HashMap memberlists;
} SimCache;

typedef struct {
	Size type_size;
	Size offset;
} SimMemberInfo;

typedef struct {
	Size memsize;
	Size size;
	SimMemberInfo info[];
} SimMemberListInfo;
