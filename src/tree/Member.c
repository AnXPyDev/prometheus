typedef struct {
    Allocator alc;
    HashMap heads;
    Vector members;
} MemberList;

typedef struct Member {
    struct Member *next;
    MemberList *owner;
    Size index;
    Identifier identifier;
    Qualifier qualifier;
    Type type;
} Member;

void MemberList_init(MemberList *this, Allocator alc) {
    this->alc = alc;
    Vector_create(&this->members, sizeof(Member*));
    HashMap_create(&this->heads, sizeof(Member*));
}

MemberList *MemberList_create(Allocator alc) {
    MemberList *this = Allocator_malloc(alc, sizeof(MemberList));
    MemberList_init(this, alc);
    return this;
}

void MemberList_deinit(MemberList *this) {
    Vector_destroy(&this->members, this->alc);
    HashMap_destroy(&this->heads, this->alc);
}

void MemberList_destroy(MemberList *this, Allocator alc) {
    MemberList_deinit(this);
    Allocator_free(alc, this);
}

Member *MemberList_add(MemberList *this, BufferView I, Qualifier Q, Type T) {
    Member **head = HashMap_ensure(&this->heads, I, this->alc);
    Member *member = Allocator_malloc(this->alc, sizeof(Member));
    member->next = *head;
    member->owner = this;
    member->index = this->members.size;
    member->identifier = Identifier_copy(I, this->alc);
    member->qualifier = Qualifier_constcast(Qualifier_copy(Q, this->alc));
    member->type = Type_constcast(Type_copy(T, this->alc));

    *head = member;

    *(Member**)Vector_push(&this->members, this->alc) = member;
    return member;
}

Member *MemberList_matching(MemberList *this, BufferView identifier) {
    Member **mp = HashMap_get(&this->heads, identifier);
    if (!mp) return NULL;
    return *mp;
}

Array MemberList_members(MemberList *this) {
    return Vector_array(&this->members);
}

#define this ((Member*)vthis)

void Member_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Member(\"{}\"; T: {}; Q: {})",
        bufrepr(Identifier_view(this->identifier)),
        Type_repr(this->type),
        Qualifier_repr(this->qualifier)
    );
}

#undef this

const IPrintable IPrintable_Member = {
    .print = &Member_print
};

Printable Member_repr(Member *this) {
    return (Printable) {
        .interface = &IPrintable_Member,
        .object = this
    };
}
