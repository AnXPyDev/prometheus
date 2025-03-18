typedef struct {
	Allocator alc;
	HashMap memberlists;
} SimCache;

enum {
	MEMBERINFO_FLAG_ANY = 1 << 0
};

typedef struct {
	Size type_size;
	Size offset;
	int flags;
} SimMemberInfo;

typedef struct {
	Size memsize;
	Size size;
	SimMemberInfo info[];
} SimMemberListInfo;
