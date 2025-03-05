typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	HashMap *mmap;
	char data[];
} SimStackFrame;
