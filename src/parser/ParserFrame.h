typedef struct ParserFrame {
	struct ParserFrame *parent;
	MemberList *memberlist;
	HashMap values;
	Allocator alc;
} ParserFrame;
