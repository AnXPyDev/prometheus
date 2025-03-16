void **ParserFrame_setupMemberWithValue(ParserFrame *this, BufferView id, Type T) {
	Member *member = MemberList_add(this->memberlist, id, Qualifier_NULL, T);
	return (void**)HashMap_add(&this->values, (BufferView) { .data = (char*)&member, .size = sizeof(Member*) }, this->alc);
}
