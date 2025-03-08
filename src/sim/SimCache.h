typedef struct {
	Allocator alc;
	HashMap memberlists;
} SimCache;

typedef struct {
	Size memsize;
	Size size;
	Size offsets[];
} SimMemberListInfo;
