typedef struct {
    Printable (*repr_)(void *this);
    Type (*resultType)(void *this, Contract *alc);

    ISimNode simext;
} INode;

typedef struct {
    const INode *interface;
    void *object;
} Node;

const Node Node_NULL = { .interface = NULL, .object = NULL };

bool Node_isNull(Node this) { return this.interface == NULL; }

Printable Node_repr(Node this) {
    if (Node_isNull(this)) return Printable_NULL;
    return this.interface->repr_(this.object);
}

Type Node_resultType(Node this, Contract *alc) {
    if (Node_isNull(this)) return PrimitiveType_upcast(PRIMITIVE_TYPE_VOID);
    return this.interface->resultType(this.object, alc);
}
