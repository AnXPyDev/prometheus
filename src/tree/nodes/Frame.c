typedef struct {
    MemberList ml;
    Node root;
} FrameNode;

void FrameNode_create(FrameNode *this, Allocator alc) {
    MemberList_create(&this->ml, alc);
    this->root = Node_NULL;
}

#define this ((FrameNode*)vthis)

void FrameNode_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, "FrameNode(MemberList {");

    {
        MemberNode *end = Vector_end(&this->ml.nodes);
        MemberNode *it = Vector_begin(&this->ml.nodes);

        for (; it < end - 1; it++) {
            Member_print(&it->member, os, BufferView_NULL);
            OutStream_puts(os, ", ");
        }
        Member_print(&it->member, os, BufferView_NULL);
    }

    PrintFmt(os, "}; {})", Node_repr(this->root));
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

#undef this

INode INode_FrameNode = {
    .repr_ = &FrameNode_repr,
    .resultType = &FrameNode_resultType,
};

Node FrameNode_upcast(FrameNode *this) {
    return (Node) { .interface = &INode_FrameNode, .object = this };
}
