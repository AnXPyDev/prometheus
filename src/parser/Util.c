void *ParserFrame_setupMemberWithValue(ParserFrame *this, BufferView id, Type T) {
	Member *member = MemberList_add(this->memberlist, id, Qualifier_NULL, T);
	return ParserFrame_ensureValue(this, member);
}
