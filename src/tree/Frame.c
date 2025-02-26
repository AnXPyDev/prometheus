typedef struct {
    MemberList ml;
} Frame;

void Frame_create(Frame *this, Allocator alc) {
    MemberList_create(&this->ml, alc);
}

#define this ((Frame*)vthis)

void Frame_print(void *vthis, OutStream os, StringView fmt) {
    OutStream_puts(os, "Frame(ML: {");

    {
        MemberNode *end = Vector_end(&this->ml.nodes);
        MemberNode *it = Vector_begin(&this->ml.nodes);

        for (; it < end - 1; it++) {
            Member_print(&it->member, os, BufferView_NULL);
            OutStream_puts(os, ", ");
        }
        Member_print(&it->member, os, BufferView_NULL);
    }

    OutStream_puts(os, "})");
}

#undef this

const IPrintable IPrintable_Frame = {
    .print = &Frame_print
};

Printable Frame_repr(Frame *this) {
    return (Printable) { .interface = &IPrintable_Frame, .object = this };
}
