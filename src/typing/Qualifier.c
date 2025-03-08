struct Qualifier;

typedef struct {
    Printable (*repr_)(void *this);
    struct Qualifier (*copy)(void *this, Allocator alc);
    void (*destroy)(void *this, Allocator alc);
    struct Qualifier (*constcast)(void *this);
    struct Qualifier (*recast)(void *this);
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
    if (Qualifier_isNull(this)) return Qualifier_NULL;
    return this.interface->copy(this.object, alc);
}

void Qualifier_destroy(Qualifier this, Allocator alc) {
    if (Qualifier_isNull(this)) return;
    this.interface->destroy(this.object, alc);
}

Qualifier Qualifier_constcast(Qualifier this) {
    if (Qualifier_isNull(this)) return Qualifier_NULL;
    if (!this.interface->constcast) return this;
    return this.interface->constcast(this.object);
}

Qualifier Qualifier_recast(Qualifier this) {
    if (Qualifier_isNull(this)) return Qualifier_NULL;
    if (!this.interface->recast) return this;
    return this.interface->recast(this.object);
}
