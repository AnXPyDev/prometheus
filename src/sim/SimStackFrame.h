typedef struct SimStackFrame {
	struct SimStackFrame *parent;
	HashMap membermap;
	char data[];
} SimStackFrame;
