struct Qualifier;

typedef struct {
    Printable (*repr_)(void *this);
    struct Qualifier (*copy)(void *this, Allocator alc);
    struct Qualifier (*move)(void *this, Allocator alc, Allocator owner);
    void (*destroy)(void *vthis, Allocator alc);
} IQualifier;

typedef struct Qualifier {
    const IQualifier *interface;
    void *object;
} Qualifier;

const Qualifier Qualifier_NULL = { 0 };

bool Qualifier_isNull(Qualifier this) { return this.interface == NULL; }

Printable Qualifier_repr(Qualifier this) {
    if (Qualifier_isNull(this)) return Printable_NULL;
    return this.interface->repr_(this.object);
}

Qualifier Qualifier_copy(Qualifier this, Allocator alc) {
    return this.interface->copy(this.object, alc);
}

void Qualifier_destroy(Qualifier this, Allocator alc) {
    this.interface->destroy(this.object, alc);
}

Qualifier Qualifier_move(Qualifier this, Allocator alc, Allocator owner) {
    if (!this.interface->move) {
        Qualifier copy = Qualifier_copy(this, alc);
        Qualifier_destroy(this, owner);
        return copy;
    }
    return this.interface->move(this.object, alc, owner);
}
