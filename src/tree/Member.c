typedef struct {
    Type type;
    Property property;
    Identifier identifier;
} Member;

typedef struct MemberNode {
    struct MemberNode *next;
    Member member;
} MemberNode;

typedef struct {
    Vector nodes;
    HashMap heads;
} MemberList;

void MemberList_create(MemberList *this, Allocator alc) {
    Vector_create(&this->nodes, alc, sizeof(MemberNode));
    HashMap_create(&this->heads, alc, sizeof(MemberNode*));
}

void MemberList_add(MemberList *this, Member *member) {
    MemberNode *node = Vector_push(&this->nodes);

    MemberNode **head = HashMap_ensure(&this->heads, member->identifier.value);

    node->next = *head;
    node->member = *member;

    *head = node; 
}

MemberNode *MemberList_getHead(MemberList *this, Identifier *identifier) {

    MemberNode *head = HashMap_get(&this->heads, identifier->value);
}
