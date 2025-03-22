struct Qualifier;

typedef struct {
    Printable (*repr_)(void *this);
    struct Qualifier (*copy)(void *this, Allocator alc);
    void (*destroy)(void *this, Allocator alc);
    struct Qualifier (*constcast)(void *this);
    struct Qualifier (*recast)(void *this);
    bool (*match)(void *this, struct Qualifier other);
    Hash (*hash)(void *this);
} IQualifier;

typedef struct Qualifier {
    const IQualifier *interface;
    void *object;
} Qualifier;

const Qualifier Qualifier_NULL = { 0 };

bool Qualifier_isNull(Qualifier this) { return this.interface == NULL; }
bool Qualifier_isPrimitive(Qualifier);
bool Qualifier_isUnionQualifier(Qualifier);


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

bool Qualifier_match(Qualifier this, Qualifier other) {
    bool this_null = Qualifier_isNull(this);
    bool other_null = Qualifier_isNull(other);

    if (this_null && other_null) return true;
    if (this_null) return false;
    if (other_null) return true;

    if (!this.interface->match) return false;

    return this.interface->match(this.object, other);
}

Hash Qualifier_hash(Qualifier this) {
    if (!this.interface->hash) return Hash_NULL;
    return this.interface->hash(this.object);
}
