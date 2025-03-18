typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	MemberList *memberlist;
	SimMemberListInfo *mlinfo;
	Vector deferred;
	Allocator deferred_alc;
	char data[];
} SimStackFrame;
