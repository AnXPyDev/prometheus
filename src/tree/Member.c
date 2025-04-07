typedef struct {
    Allocator alc;
    HashMap *heads;
    Vector members;
    void *owner;
    void *info;
} MemberList;

typedef struct Member {
    struct Member *next;
    MemberList *owner;
    Size index;
    Identifier identifier;
    Qualifier qualifier;
    Type type;
} Member;

bool MemberList_match_head(void *object, void *payload) {
    BufferView I = *(BufferView*)payload;
    Member *member = *(Member**)object;
    return BufferView_isEqual(I, Identifier_view(member->identifier));
}

HashMap_Key MemberList_headKey(BufferView *identifier) {
    return (HashMap_Key) {
        .match = &MemberList_match_head,
        .payload = identifier
    };
}

void MemberList_init(MemberList *this, Allocator alc) {
    this->alc = alc;
    Vector_create(&this->members, sizeof(Member*));
    this->heads = HashMap_create(16, alc);
}

void MemberList_deinit(MemberList *this) {
    Vector_destroy(&this->members, this->alc);
    HashMap_destroy(this->heads, this->alc);
}

Member *MemberList_add(MemberList *this, BufferView I, Qualifier Q, Type T) {
    Member **head = HashMap_ensure(
        this->heads, BufferView_hash(I), sizeof(Member*),
        MemberList_headKey(&I), this->alc
    );

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
    Member **mp = HashMap_get(this->heads,
        BufferView_hash(identifier),
        MemberList_headKey(&identifier)
    );
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

#define this ((MemberList*)vthis)

void MemberList_print(void *vthis, OutStream os, StringView fmt) {
    if (this->members.size == 0) return;

    Member **it = Vector_begin(&this->members);
    Member **end = Vector_end(&this->members);

    for (; it < end - 1; it++) {
        Member_print(*it, os, BufferView_NULL);
        OutStream_puts(os, ", ");
    }
    Member_print(*it, os, BufferView_NULL);
}

#undef this

const IPrintable IPrintable_MemberList = {
    .print = &MemberList_print
};

Printable MemberList_repr(MemberList *this) {
    return (Printable) {
        .interface = &IPrintable_MemberList,
        .object = this
    };
}

void MemberList_destroy(MemberList*);
MemberList *MemberList_constcopy(MemberList*);
