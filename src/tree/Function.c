typedef struct {
	MemberList arguments;
	Node node;
} Function;

void Function_create(Function *this, Allocator alc) {
	MemberList_create(&this->arguments, alc);
}
