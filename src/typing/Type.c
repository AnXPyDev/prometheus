typedef struct {
    Printable (*repr_)(void *this);
} IType;

typedef struct {
    const IType *interface;
    void *object;
} Type;

Printable Type_repr(Type this) {
    return this.interface->repr_(this.object);
}
