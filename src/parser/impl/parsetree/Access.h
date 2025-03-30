typedef struct {
	ParseTreeState_NODE node;
	ptrdiff_t offset;
	Type type;
} ParseTreeState_NODE_ELEMENT;

typedef struct {
	ParseTreeState_MEMBER member;
	ptrdiff_t offset;
	Type type;
} ParseTreeState_MEMBER_ELEMENT;
