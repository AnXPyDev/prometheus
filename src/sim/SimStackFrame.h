typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	MemberList *memberlist;
	MemberListInfo *mlinfo;
	Vector deferred;
	Allocator deferred_alc;
	char data[];
} SimStackFrame;
