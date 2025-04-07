void *ParserFrame_setupMemberWithValue(ParserFrame *this, BufferView id, Qualifier Q, Type T) {
	Member *member = MemberList_add(this->memberlist, id, Q, T);
	return ParserFrame_ensureValue(this, member);
}
