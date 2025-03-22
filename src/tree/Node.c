struct Node;

typedef struct {
    Printable (*repr_)(void *this);
    Type (*resultType)(void *this, Allocator alc);
    void (*destroy)(void *this, Allocator alc);
    struct Node (*copy)(void *this, Allocator alc);

    ISimNode simext;
    IParserNode pext;
} INode;

typedef struct Node {
    const INode *interface;
    void *object;
} Node;

const Node Node_NULL = { .interface = NULL, .object = NULL };

bool Node_isNull(Node this) { return this.interface == NULL; }

Printable Node_repr(Node this) {
    if (Node_isNull(this)) return Printable_NULL;
    return this.interface->repr_(this.object);
}

Type Node_resultType(Node this, Allocator alc) {
    if (Node_isNull(this)) return Type_NULL;
    return this.interface->resultType(this.object, alc);
}

void Node_destroy(Node this, Allocator alc) {
    if (Node_isNull(this)) return;
    this.interface->destroy(this.object, alc);
}

Node Node_copy(Node this, Allocator alc) {
    if (Node_isNull(this)) return Node_NULL;
    return this.interface->copy(this.object, alc);
}
