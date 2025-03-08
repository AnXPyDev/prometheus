typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	MemberList *memberlist;
	SimMemberListInfo *mlinfo;
	char data[];
} SimStackFrame;
