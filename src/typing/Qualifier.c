typedef struct {
    Printable (*repr_)(void *this);
} IQualifier;

typedef struct {
    const IQualifier *interface;
    void *object;
} Qualifier;

const Qualifier Qualifier_NULL = { 0 };

bool Qualifier_isNull(Qualifier this) { return this.interface == NULL; }

Printable Qualifier_repr(Qualifier this) {
    if (Qualifier_isNull(this)) return Printable_NULL;
    return this.interface->repr_(this.object);
}

