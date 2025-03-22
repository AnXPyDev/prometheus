typedef struct {
	MemberList *arguments;
	Node node;
	Type type;
} Function;

typedef struct {
	Function *function;
	void *closure;
} FunctionValue;
