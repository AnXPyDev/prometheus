typedef struct {
    Type type;
    Qualifier qualifier;
    Identifier identifier;
} Member;

typedef struct MemberNode {
    struct MemberNode *next;
    Member member;
} MemberNode;

typedef struct {
    Size size;
    MemberNode *node;
} MemberHead; 

typedef struct {
    Vector nodes;
    HashMap heads;
} MemberList;

void MemberList_create(MemberList *this, Allocator alc) {
    Vector_create(&this->nodes, alc, sizeof(MemberNode));
    HashMap_create(&this->heads, alc, sizeof(MemberHead));
}

void MemberList_add(MemberList *this, Member *member) {
    MemberNode *node = Vector_push(&this->nodes);

    MemberHead *head = HashMap_ensure(&this->heads, member->identifier.value);
    node->next = head->node;
    node->member = *member;
    head->node = node;
    head->size++;
}

MemberHead *MemberList_getHead(MemberList *this, Identifier *identifier) {
    return HashMap_get(&this->heads, identifier->value);
}

Size MemberList_getSize(MemberList *this) {
    return this->nodes.size;
}

void MemberList_getMembers(MemberList *this, Member **out_members) {
    MemberNode *end = Vector_end(&this->nodes);
    for (MemberNode *it = Vector_begin(&this->nodes); it < end; it++) {
        *(out_members++) = &it->member;
    }
}

void MemberHead_getMembers(MemberHead *head, Member **out_members) {
    MemberNode *node = head->node;
    while (node) {
        *(out_members++) = &node->member;
        node = node->next;
    }
}

#define this ((Member*)vthis)

void Member_print(void *vthis, OutStream os, StringView fmt) {
    PrintFmt(os, "Member(\"{}\"; T: {}; Q: {})",
        Identifier_repr(&this->identifier),
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
