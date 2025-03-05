typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	void *origin;
	HashMap *mmap;
	char data[];
} SimStackFrame;
