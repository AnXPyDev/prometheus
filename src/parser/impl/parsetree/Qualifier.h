typedef struct {
	ParseTreeState header;
	Qualifier qualifier;
} ParseTreeState_QUALIFIER;

typedef struct {
	ParseTreeState header;
	Type type;
	Qualifier qualifier;
} ParseTreeState_TYPE_AND_QUALIFIER;
