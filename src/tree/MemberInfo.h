enum {
	MEMBERINFO_FLAG_ANY = 1 << 0
};

typedef struct {
	TypeInfo typeinfo;
	Hash hash;
	Size offset;
	int flags;
} MemberInfo;

typedef struct {
	TypeInfo typeinfo;
	Size size;
	Hash hash;
	MemberInfo info[];
} MemberListInfo;
