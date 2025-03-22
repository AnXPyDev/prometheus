typedef struct {
    MemberList *memberlist;
    Node root;
    void *values[];
} FrameNode;

Node FrameNode_upcast(FrameNode*);
Node FrameNode_create(MemberList *memberlist, Node root, Allocator alc) {
    FrameNode *this = Allocator_calloc(alc, sizeof(FrameNode) + sizeof(void*) * memberlist->members.size);
    this->root = root;
    this->memberlist = MemberList_constcopy(memberlist);
    return FrameNode_upcast(this);
}

#define this ((FrameNode*)vthis)

void FrameNode_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, "Frame([");
    MemberList_print(this->memberlist, os, BufferView_NULL);
    PrintFmt(os, "]; {})", Node_repr(this->root));
}

const IPrintable IPrintable_FrameNode = {
    .print = &FrameNode_print
};

Printable FrameNode_repr(void *vthis) {
    return (Printable) { .interface = &IPrintable_FrameNode, .object = vthis };
}

Type FrameNode_resultType(void *vthis, Allocator alc) {
    return Node_resultType(this->root, alc);
}

void FrameNode_destroy(void *vthis, Allocator alc) {
    MemberList_destroy(this->memberlist);
    Node_destroy(this->root, alc);
    Allocator_free(alc, vthis);
}

Node FrameNode_copy(void *vthis, Allocator alc) {
    Size msize = this->memberlist->members.size;
    FrameNode *copy = Allocator_calloc(alc, sizeof(FrameNode) + sizeof(void*) * msize);
    copy->root = Node_copy(this->root, alc);
    copy->memberlist = MemberList_constcopy(this->memberlist);
    memcpy(copy->values, this->values, sizeof(void*) * msize);
    return FrameNode_upcast(this);
}

#undef this

INode INode_FrameNode = {
    .repr_ = &FrameNode_repr,
    .resultType = &FrameNode_resultType,
    .destroy = &FrameNode_destroy,
    .copy = &FrameNode_copy
};

Node FrameNode_upcast(FrameNode *this) {
    return (Node) { .interface = &INode_FrameNode, .object = this };
}
